// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_PASS_HPP
#define VOXEL_FLOW_RENDER_PASS_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderTargetGroup.hpp>

namespace VoxFlow
{
class LogicalDevice;

class RenderPass : NonCopyable
{
 public:
    explicit RenderPass(LogicalDevice* logicalDevice);
    ~RenderPass() override;
    RenderPass(RenderPass&& other) noexcept;
    RenderPass& operator=(RenderPass&& other) noexcept;

    [[nodiscard]] inline VkRenderPass get() const noexcept
    {
        return _renderPass;
    }

 public:
    bool initialize(const RenderTargetLayoutKey& rtLayoutKey);
    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    VkRenderPass _renderPass{ VK_NULL_HANDLE };
    RenderTargetLayoutKey _renderTargetLayout;
};
}  // namespace VoxFlow

#endif