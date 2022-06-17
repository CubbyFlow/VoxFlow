// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_PASS_HPP
#define VOXEL_FLOW_RENDER_PASS_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;

class RenderPass : NonCopyable
{
 public:
    explicit RenderPass(const std::shared_ptr<LogicalDevice>& device);
    ~RenderPass() override;
    RenderPass(RenderPass&& other) noexcept;
    RenderPass& operator=(RenderPass&& other) noexcept;

    [[nodiscard]] VkRenderPass get() const noexcept
    {
        return _renderPass;
    }

 protected:
    void release();

 private:
    std::shared_ptr<LogicalDevice> _device;
    VkRenderPass _renderPass{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif