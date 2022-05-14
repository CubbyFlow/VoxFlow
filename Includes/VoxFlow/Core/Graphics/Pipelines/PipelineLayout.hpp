// Author : snowapril

#ifndef VOXEL_FLOW_PIPELINE_LAYOUT_HPP
#define VOXEL_FLOW_PIPELINE_LAYOUT_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;

class PipelineLayout : NonCopyable
{
 public:
    explicit PipelineLayout(const std::shared_ptr<LogicalDevice>& device);
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
    std::shared_ptr<LogicalDevice> _device;
    VkPipelineLayout _layout{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif