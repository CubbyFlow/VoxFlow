// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_LAYOUT_HPP
#define VOXEL_FLOW_PIPELINE_LAYOUT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;
class DescriptorSetLayout;

class PipelineLayout : NonCopyable
{
 public:
    explicit PipelineLayout(
        LogicalDevice* logicalDevice,
        const std::vector<std::shared_ptr<DescriptorSetLayout>>& setLayouts);
    ~PipelineLayout() override;
    PipelineLayout(PipelineLayout&& other) noexcept;
    PipelineLayout& operator=(PipelineLayout&& other) noexcept;

    [[nodiscard]] VkPipelineLayout get() const noexcept
    {
        return _layout;
    }

 protected:
    void release();

 private:
    LogicalDevice* _logicalDevice;
    VkPipelineLayout _layout{ VK_NULL_HANDLE };
    std::vector<std::shared_ptr<DescriptorSetLayout>> _setLayouts;
};
}  // namespace VoxFlow

#endif