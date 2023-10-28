// Author : snowapril

#ifndef VOXEL_FLOW_SHADER_MODULE_HPP
#define VOXEL_FLOW_SHADER_MODULE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayoutDescriptor.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <array>
#include <unordered_map>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;

class ShaderModule : private NonCopyable
{
 public:
    explicit ShaderModule(LogicalDevice* logicalDevice,
                          const std::string& shaderFilePath);
    ~ShaderModule() override;
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule& operator=(ShaderModule&& other) noexcept;

    [[nodiscard]] inline VkShaderModule get() const noexcept
    {
        return _shaderModule;
    }

    /**
     * @return shader stage of this module
     */
    [[nodiscard]] inline VkShaderStageFlagBits getStageFlagBits() const
    {
        return _stageFlagBits;
    }

    /**
     * @return reflected shader layout binding of thie module
     */
    [[nodiscard]] inline const ShaderReflectionDataGroup*
    getShaderReflectionDataGroup() const
    {
        return &_reflectionDataGroup;
    }

    /**
     * release shader module object
     */
    void release();

 private:
    /**
     * Reflect shader layout bindings from spirv binary data.
     * stage input/output, push constants, shader resource bindings for each
     * slot category will be reflected
     * @return whether reflection is successful or not
     */
    static bool reflectShaderLayoutBindings(
        ShaderReflectionDataGroup* reflectionDataGroup,
        std::vector<uint32_t>&& spirvCodes, VkShaderStageFlagBits stageBits);

 protected:
    LogicalDevice* _logicalDevice;
    VkShaderModule _shaderModule = VK_NULL_HANDLE;
    ShaderReflectionDataGroup _reflectionDataGroup;
    std::string _shaderFilePath;
    VkShaderStageFlagBits _stageFlagBits;
};
}  // namespace VoxFlow

#endif