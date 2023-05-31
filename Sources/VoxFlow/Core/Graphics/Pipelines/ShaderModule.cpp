// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <spirv-cross/spirv_common.hpp>
#include <spirv-cross/spirv_cross.hpp>

namespace VoxFlow
{

ShaderModule::ShaderModule(LogicalDevice* logicalDevice,
                           const char* shaderFilePath)
    : _logicalDevice(logicalDevice), _shaderFilePath(shaderFilePath)
{
    std::vector<char> shaderSource;
    VOX_ASSERT(GlslangUtil::ReadShaderFile(shaderFilePath, &shaderSource),
               "Failed to read shader file : {}", shaderFilePath);

    const glslang_stage_t glslangStage =
        GlslangUtil::GlslangStageFromFilename(shaderFilePath);
    std::vector<unsigned int> spirvBinary;
    VOX_ASSERT(GlslangUtil::CompileShader(glslangStage, shaderSource.data(),
                                          &spirvBinary),
               " Failed to compile shader file : {}", shaderFilePath);

    _stageFlagBits = GlslangUtil::GlslangStageToVulkanStage(glslangStage);

    const VkShaderModuleCreateInfo moduleInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = spirvBinary.size() * sizeof(unsigned int),
        .pCode = spirvBinary.data()
    };
    VK_ASSERT(vkCreateShaderModule(_logicalDevice->get(), &moduleInfo, nullptr,
                                   &_shaderModule));

    reflectShaderLayoutBindings(std::move(spirvBinary));

    _setLayout =
        std::make_shared<DescriptorSetLayout>(_logicalDevice, _layoutBindings);
}

ShaderModule::~ShaderModule()
{
    release();
}

ShaderModule::ShaderModule(ShaderModule&& other) noexcept
{
    operator=(std::move(other));
}

ShaderModule& ShaderModule::operator=(ShaderModule&& other) noexcept
{
    if (this != &other)
    {
        _logicalDevice = other._logicalDevice;
        _shaderModule = other._shaderModule;
        _layoutBindings.swap(other._layoutBindings);
        _shaderFilePath = other._shaderFilePath;
        _stageFlagBits = other._stageFlagBits;

        other._shaderModule = VK_NULL_HANDLE;
    }
    return *this;
}

void ShaderModule::release()
{
    if (_shaderModule != VK_NULL_HANDLE)
    {
        vkDestroyShaderModule(_logicalDevice->get(), _shaderModule, nullptr);
    }
}

bool ShaderModule::reflectShaderLayoutBindings(
    std::vector<uint32_t>&& spirvCodes)
{
    // Note(snowapril) : sample codes from Khronos/SPIRV-Cross Wiki.
    //                   https://github.com/KhronosGroup/SPIRV-Cross/wiki/Reflection-API-user-guide

    // Creating reflection object
    spirv_cross::Compiler compiler(std::move(spirvCodes));

    // Querying statically accessed resources
    std::unordered_set<spirv_cross::VariableID> activeVariableSet =
        compiler.get_active_interface_variables();
    spirv_cross::ShaderResources shaderResources =
        compiler.get_shader_resources(activeVariableSet);
    compiler.set_enabled_interface_variables(std::move(activeVariableSet));

    spdlog::debug("[SPIRV Reflection ({})]", _shaderFilePath);

    const auto& reflectShaderResources =
        [&compiler, this](
            const std::string_view& targetResourceType,
            spirv_cross::SmallVector<spirv_cross::Resource>& resources,
            VkDescriptorType descriptorType) {
            spdlog::debug("[{} List]", targetResourceType);

            for (const spirv_cross::Resource& resource : resources)
            {
                const uint32_t set = compiler.get_decoration(
                    resource.id, spv::DecorationDescriptorSet);
                const uint32_t binding = compiler.get_decoration(
                    resource.id, spv::DecorationBinding);

                const spirv_cross::SPIRType& resourceType =
                    compiler.get_type(resource.type_id);
                const uint32_t count =
                    resourceType.array[0] == 0 ? 1 : resourceType.array[0];

                spdlog::debug(
                    "\t {} (set : {}, binding : {}, count : {})", resource.name,
                    set, binding,
                    count);  // TODO(snowapril) : support multi-dimensional

                _layoutBindings.push_back(ShaderLayoutBinding(
                    resource.name, set,
                    VkDescriptorSetLayoutBinding{
                        .binding = binding,
                        .descriptorType = descriptorType,
                        .descriptorCount = count,
                        .stageFlags =
                            static_cast<VkShaderStageFlags>(_stageFlagBits),
                        .pImmutableSamplers = nullptr }));
            }
        };

    reflectShaderResources("Combined Sampled Image List",
                           shaderResources.sampled_images,
                           VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER);

    reflectShaderResources("Storage Texel Buffer List",
                           shaderResources.storage_images,
                           VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER);

    return true;
}

}  // namespace VoxFlow