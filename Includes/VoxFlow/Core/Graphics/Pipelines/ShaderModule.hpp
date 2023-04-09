// Author : snowapril

#ifndef VOXEL_FLOW_SHADER_MODULE_HPP
#define VOXEL_FLOW_SHADER_MODULE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderLayoutBinding.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;
class DescriptorSetLayout;

class ShaderModule : private NonCopyable
{
 public:
    explicit ShaderModule(LogicalDevice* logicalDevice,
                          const char* shaderFilePath);
    ~ShaderModule() override;
    ShaderModule(ShaderModule&& other) noexcept;
    ShaderModule& operator=(ShaderModule&& other) noexcept;

    [[nodiscard]] inline VkShaderModule get() const noexcept
    {
        return _shaderModule;
    }

    [[nodiscard]] inline VkShaderStageFlagBits getStageFlagBits() const
    {
        return _stageFlagBits;
    }

    [[nodiscard]] inline const std::shared_ptr<DescriptorSetLayout>&
    getDescriptorSetLayout() const
    {
        return _setLayout;
    }

    inline const std::vector<ShaderLayoutBinding>&
    getShaderLayoutBindings() const
    {
        return _layoutBindings;
    }

    void release();

 private:
    bool reflectShaderLayoutBindings(std::vector<uint32_t>&& spirvCodes);

 protected:
    LogicalDevice* _logicalDevice;
    VkShaderModule _shaderModule = VK_NULL_HANDLE;
    std::vector<ShaderLayoutBinding> _layoutBindings;
    const char* _shaderFilePath = nullptr;
    VkShaderStageFlagBits _stageFlagBits;
    std::shared_ptr<DescriptorSetLayout> _setLayout;
};
}  // namespace VoxFlow

#endif