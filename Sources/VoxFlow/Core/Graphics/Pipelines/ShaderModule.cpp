// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStreamingContext.hpp>
#include <VoxFlow/Core/Utils/VertexFormat.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <spirv-cross/spirv_common.hpp>
#include <spirv-cross/spirv_cross.hpp>

namespace VoxFlow
{
ShaderModule::ShaderModule(PipelineStreamingContext* pipelineStreamingContext,
                           const ShaderPathInfo& shaderPath)
    : _pipelineStreamingContext(pipelineStreamingContext),
      _logicalDevice(_pipelineStreamingContext->getLogicalDevice()),
      _shaderFilePath(shaderPath)
{
    std::vector<unsigned int> spirvBinary;
    const bool compileResult = _pipelineStreamingContext->loadSpirvBinary(spirvBinary, shaderPath);
    VOX_ASSERT(compileResult, "Failed to load spirv binary for path : {}",
               shaderPath.path);

    if (compileResult == false)
        return;

    const VkShaderModuleCreateInfo moduleInfo = {
        .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .codeSize = spirvBinary.size() * sizeof(unsigned int),
        .pCode = spirvBinary.data()
    };
    VK_ASSERT(vkCreateShaderModule(_logicalDevice->get(), &moduleInfo, nullptr,
                                   &_shaderModule));

    _stageFlagBits =
        ShaderUtil::ConvertToShaderStageFlag(_shaderFilePath.shaderStage);

    const bool reflectionResult = reflectShaderLayoutBindings(
        &_reflectionDataGroup, std::move(spirvBinary), _stageFlagBits);
    VOX_ASSERT(reflectionResult, "Failed to reflect shader module {}",
               _shaderFilePath.path);
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
        _pipelineStreamingContext = other._pipelineStreamingContext;
        _shaderModule = other._shaderModule;
        _reflectionDataGroup = std::move(other._reflectionDataGroup);
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

static uint32_t getSpirTypeSize(const spirv_cross::SPIRType& resourceType, bool& isBindlessResource)
{
    uint32_t size = 0;
    isBindlessResource = false;
    if (!resourceType.array.empty())
    {
        if (resourceType.array.size() != 1)
        {
            spdlog::error("Array dimension must be 1");
        }
        else if (!resourceType.array_size_literal.front())
        {
            spdlog::error("Array dimension must be literal value");
        }
        else
        {
            if (resourceType.array.front() == 0)
            {
                isBindlessResource = true;
                size = UINT32_MAX;
            }
            else
            {
                size = uint8_t(resourceType.array.front());
            }
        }
    }
    else
    {
        size = 1;
    }

    return size;
}

static VkFormat convertSpirvImageFormat(spv::ImageFormat imageFormat)
{
    switch (imageFormat)
    {
        case spv::ImageFormatUnknown:
            return VK_FORMAT_UNDEFINED;
        case spv::ImageFormatRgba32f:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case spv::ImageFormatRgba16f:
            return VK_FORMAT_R4G4B4A4_UNORM_PACK16;
        case spv::ImageFormatR32f:
            return VK_FORMAT_R32_SFLOAT;
        case spv::ImageFormatRgba8:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case spv::ImageFormatRgba8Snorm:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case spv::ImageFormatRg32f:
            return VK_FORMAT_R32G32_SFLOAT;
        case spv::ImageFormatRg16f:
            return VK_FORMAT_R16G16_SFLOAT;
        case spv::ImageFormatR11fG11fB10f:
            return VK_FORMAT_B10G11R11_UFLOAT_PACK32;
        case spv::ImageFormatR16f:
            return VK_FORMAT_R16_SFLOAT;
        case spv::ImageFormatRgba16:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case spv::ImageFormatRgb10A2:
            return VK_FORMAT_A2R10G10B10_UNORM_PACK32;
        case spv::ImageFormatRg16:
            return VK_FORMAT_R16G16_UNORM;
        case spv::ImageFormatRg8:
            return VK_FORMAT_R8G8_UNORM;
        case spv::ImageFormatR16:
            return VK_FORMAT_R16_UNORM;
        case spv::ImageFormatR8:
            return VK_FORMAT_R8_UNORM;
        case spv::ImageFormatRgba16Snorm:
            return VK_FORMAT_R16G16B16A16_SNORM;
        case spv::ImageFormatRg16Snorm:
            return VK_FORMAT_R16G16_SNORM;
        case spv::ImageFormatRg8Snorm:
            return VK_FORMAT_R8G8_SNORM;
        case spv::ImageFormatR16Snorm:
            return VK_FORMAT_R16_SNORM;
        case spv::ImageFormatR8Snorm:
            return VK_FORMAT_R8_SNORM;
        case spv::ImageFormatRgba32i:
            return VK_FORMAT_R32G32B32A32_UINT;
        case spv::ImageFormatRgba16i:
            return VK_FORMAT_R16G16B16A16_UINT;
        case spv::ImageFormatRgba8i:
            return VK_FORMAT_R8G8B8A8_SINT;
        case spv::ImageFormatR32i:
            return VK_FORMAT_R32_SINT;
        case spv::ImageFormatRg32i:
            return VK_FORMAT_R32G32_SINT;
        case spv::ImageFormatRg16i:
            return VK_FORMAT_R16G16_SINT;
        case spv::ImageFormatRg8i:
            return VK_FORMAT_R8G8_SINT;
        case spv::ImageFormatR16i:
            return VK_FORMAT_R16_SINT;
        case spv::ImageFormatR8i:
            return VK_FORMAT_R8_SINT;
        case spv::ImageFormatRgba32ui:
            return VK_FORMAT_R32G32B32A32_UINT;
        case spv::ImageFormatRgba16ui:
            return VK_FORMAT_R16G16B16A16_UINT;
        case spv::ImageFormatRgba8ui:
            return VK_FORMAT_R8G8B8A8_UINT;
        case spv::ImageFormatR32ui:
            return VK_FORMAT_R32_UINT;
        case spv::ImageFormatRgb10a2ui:
            return VK_FORMAT_A2R10G10B10_UINT_PACK32;
        case spv::ImageFormatRg32ui:
            return VK_FORMAT_R32G32_UINT;
        case spv::ImageFormatRg16ui:
            return VK_FORMAT_R16G16_UINT;
        case spv::ImageFormatRg8ui:
            return VK_FORMAT_R8G8_UINT;
        case spv::ImageFormatR16ui:
            return VK_FORMAT_R16_UINT;
        case spv::ImageFormatR8ui:
            return VK_FORMAT_R8_UINT;
        case spv::ImageFormatR64ui:
            return VK_FORMAT_R64_UINT;
        case spv::ImageFormatR64i:
            return VK_FORMAT_R64_SINT;
        default:
            spdlog::error("Unknown image format was givne {}", imageFormat);
            return VK_FORMAT_UNDEFINED;
    }
}

VertexFormatBaseType convertToBaseType(
    const spirv_cross::SPIRType::BaseType& spirBaseType)
{
    using namespace spirv_cross;
    VertexFormatBaseType baseType = VertexFormatBaseType::Unknown;
    switch (spirBaseType)
    {
        case SPIRType::BaseType::Float:
            baseType = VertexFormatBaseType::Float32;
            break;
        case SPIRType::BaseType::Double:
            baseType = VertexFormatBaseType::Float64;
            break;
        case SPIRType::BaseType::Int:
            baseType = VertexFormatBaseType::Int32;
            break;
        case SPIRType::BaseType::Int64:
            baseType = VertexFormatBaseType::Int64;
            break;
        case SPIRType::BaseType::UInt:
            baseType = VertexFormatBaseType::Uint32;
            break;
        case SPIRType::BaseType::UInt64:
            baseType = VertexFormatBaseType::Uint64;
            break;
        default:
            break;
    }
    return baseType;
}

bool ShaderModule::reflectShaderLayoutBindings(ShaderReflectionDataGroup* reflectionDataGroup,
    std::vector<uint32_t>&& spirvCodes, VkShaderStageFlagBits shaderStageBits)
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

    bool isBindless = false;
    for (const spirv_cross::Resource& resource : shaderResources.sampled_images)
    {
        const uint32_t set =
            compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        const uint32_t binding =
            compiler.get_decoration(resource.id, spv::DecorationBinding);
        VOX_ASSERT(set < MAX_NUM_SET_SLOTS, "Set number must be under {}",
                   MAX_NUM_SET_SLOTS);

        const spirv_cross::SPIRType& resourceType =
            compiler.get_type(resource.type_id);

        const uint32_t count =
            getSpirTypeSize(resourceType, isBindless);
        VOX_ASSERT(
            (isBindless == false) ||
                (set == static_cast<uint32_t>(SetSlotCategory::Bindless)),
            "Bindless resource must use set = {}",
            static_cast<uint32_t>(SetSlotCategory::Bindless));

        //const std::string& blockName = compiler.get_name(resource.base_type_id);

        spdlog::debug("\t {} (set : {}, binding : {}, count : {})",
                      resource.name, set, binding, count);

        if (resourceType.image.dim == spv::DimBuffer)
        {
            // layout.sets[set].sampled_texel_buffer_mask |= 1u << binding;
        }
        else
        {
            VkFormat imageFormat =
                convertSpirvImageFormat(resourceType.image.format);
            (void)imageFormat;

            reflectionDataGroup->_descriptors.emplace(
                DescriptorInfo{ static_cast<SetSlotCategory>(set),
                                DescriptorCategory::CombinedImage, count,
                                binding },
                resource.name);
        }
    }

    for (const spirv_cross::Resource& resource :
         shaderResources.uniform_buffers)
    {
        const uint32_t set =
            compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        const uint32_t binding =
            compiler.get_decoration(resource.id, spv::DecorationBinding);
        VOX_ASSERT(set < MAX_NUM_SET_SLOTS, "Set number must be under {}",
                   MAX_NUM_SET_SLOTS);

        const spirv_cross::SPIRType& resourceType =
            compiler.get_type(resource.type_id);

        const uint32_t count =
            getSpirTypeSize(resourceType, isBindless);
        VOX_ASSERT(
            (isBindless == false) ||
                (set == static_cast<uint32_t>(SetSlotCategory::Bindless)),
            "Bindless resource must use set = {}",
            static_cast<uint32_t>(SetSlotCategory::Bindless));

        spdlog::debug("\t {} (set : {}, binding : {}, count : {})",
                      resource.name, set, binding, count);

        uint32_t totalSize = 0;
        for (uint32_t i = 0; i < resourceType.member_types.size(); ++i)
        {
            // auto& memberType = compiler.get_type(resourceType.member_types[i]);
            size_t memberSize =
                compiler.get_declared_struct_member_size(resourceType, i);
            size_t offset = compiler.type_struct_member_offset(resourceType, i);
            const std::string& memberName =
                compiler.get_member_name(resourceType.self, i);

            spdlog::debug("\t\t member({} : size({}), offset({})", memberName,
                          memberSize, offset);
            totalSize += static_cast<uint32_t>(memberSize);
        }
        const std::string& blockName = compiler.get_name(resource.base_type_id);
        spdlog::debug("\t Block : {}, totalSize : {}", blockName, totalSize);

        reflectionDataGroup->_descriptors.emplace(
            DescriptorInfo{ static_cast<SetSlotCategory>(set),
                            DescriptorCategory::UniformBuffer, count, binding },
            resource.name);
    }

    for (const spirv_cross::Resource& resource :
         shaderResources.storage_buffers)
    {
        const uint32_t set =
            compiler.get_decoration(resource.id, spv::DecorationDescriptorSet);
        const uint32_t binding =
            compiler.get_decoration(resource.id, spv::DecorationBinding);
        VOX_ASSERT(set < MAX_NUM_SET_SLOTS, "Set number must be under {}",
                   MAX_NUM_SET_SLOTS);

        const spirv_cross::SPIRType& resourceType =
            compiler.get_type(resource.type_id);

        const uint32_t count =
            getSpirTypeSize(resourceType, isBindless);
        VOX_ASSERT(
            (isBindless == false) ||
                (set == static_cast<uint32_t>(SetSlotCategory::Bindless)),
            "Bindless resource must use set = {}",
            static_cast<uint32_t>(SetSlotCategory::Bindless));

        spdlog::debug("\t {} (set : {}, binding : {}, count : {})",
                      resource.name, set, binding, count);

        uint32_t totalSize = 0;
        for (uint32_t i = 0; i < resourceType.member_types.size(); ++i)
        {
            // auto& memberType = compiler.get_type(resourceType.member_types[i]);
            size_t memberSize =
                compiler.get_declared_struct_member_size(resourceType, i);
            size_t offset = compiler.type_struct_member_offset(resourceType, i);
            const std::string& memberName =
                compiler.get_member_name(resourceType.self, i);

            spdlog::debug("\t\t member({} : size({}), offset({})", memberName,
                          memberSize, offset);
            totalSize += static_cast<uint32_t>(memberSize);
        }
        const std::string& blockName = compiler.get_name(resource.base_type_id);
        spdlog::debug("\t Block : {}, totalSize : {}", blockName, totalSize);

        reflectionDataGroup->_descriptors.emplace(
            DescriptorInfo{ static_cast<SetSlotCategory>(set),
                            DescriptorCategory::StorageBuffer, count, binding },
            resource.name);
    }

    if (shaderStageBits == VK_SHADER_STAGE_VERTEX_BIT)
    {
        for (const spirv_cross::Resource& attribute :
             shaderResources.stage_inputs)
        {
            auto location =
                compiler.get_decoration(attribute.id, spv::DecorationLocation);

            auto binding =
                compiler.get_decoration(attribute.id, spv::DecorationBinding);

            const spirv_cross::SPIRType& resourceType =
                compiler.get_type(attribute.type_id);

            const uint32_t size = static_cast<uint32_t>(resourceType.width *
                                                        resourceType.vecsize);
            VOX_ASSERT(resourceType.columns == 1,
                       "Matrix should not be used in stage input/output");

            VertexFormatBaseType baseType =
                convertToBaseType(resourceType.basetype);

            reflectionDataGroup->_vertexInputLayouts.push_back(
                { ._location = location,
                  ._binding = binding,
                  ._stride = (size >> 3),
                  ._baseType = baseType });
        }
        std::sort(reflectionDataGroup->_vertexInputLayouts.begin(),
                  reflectionDataGroup->_vertexInputLayouts.end(),
                  [](const auto& lhs, const auto& rhs) {
                      return lhs._location <= rhs._location;
                  });
    }
    
    if (shaderStageBits == VK_SHADER_STAGE_FRAGMENT_BIT)
    {
        for (const spirv_cross::Resource& attribute :
             shaderResources.stage_outputs)
        {
            auto location =
                compiler.get_decoration(attribute.id, spv::DecorationLocation);

            const spirv_cross::SPIRType& resourceType =
                compiler.get_type(attribute.type_id);

            VkFormat imageFormat =
                convertSpirvImageFormat(resourceType.image.format);

            reflectionDataGroup->_fragmentOutputLayouts.push_back(
                { ._location = location, ._format = imageFormat });
        }
        std::sort(reflectionDataGroup->_fragmentOutputLayouts.begin(),
                  reflectionDataGroup->_fragmentOutputLayouts.end(),
                  [](const auto& lhs, const auto& rhs) {
                      return lhs._location <= rhs._location;
                  });
    }

    if (!shaderResources.push_constant_buffers.empty())
    {
        reflectionDataGroup->_pushConstantSize = static_cast<uint32_t>(
            compiler.get_declared_struct_size(compiler.get_type(
                shaderResources.push_constant_buffers.front().base_type_id)));
    }

    reflectionDataGroup->_stageFlagBit = shaderStageBits;

    return true;
}

}  // namespace VoxFlow