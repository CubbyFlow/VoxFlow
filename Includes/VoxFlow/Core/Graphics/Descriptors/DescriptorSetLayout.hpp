// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_LAYOUT_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_LAYOUT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderLayoutBinding.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;

class DescriptorSetLayout : private NonCopyable
{
 public:
    explicit DescriptorSetLayout(
        LogicalDevice* logicalDevice,
        const std::vector<ShaderLayoutBinding>& shaderLayoutBindings);
    ~DescriptorSetLayout();
    DescriptorSetLayout(DescriptorSetLayout&& other) noexcept;
    DescriptorSetLayout& operator=(DescriptorSetLayout&& other) noexcept;

 public:
    [[nodiscard]] inline VkDescriptorSetLayout get() const
    {
        return _setLayout;
    }

 private:
    LogicalDevice* _logicalDevice = nullptr;
    std::vector<ShaderLayoutBinding> _shaderLayoutBindings;
    VkDescriptorSetLayout _setLayout = VK_NULL_HANDLE;
};
}  // namespace VoxFlow

#endif