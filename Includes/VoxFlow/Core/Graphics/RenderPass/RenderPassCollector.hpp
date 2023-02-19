// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_PASS_COLLECTOR_HPP
#define VOXEL_FLOW_RENDER_PASS_COLLECTOR_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <unordered_map>

namespace VoxFlow
{
class LogicalDevice;
class RenderPass;
class FrameBuffer;
class Texture;

class RenderPassCollector : private NonCopyable
{
 public:
    explicit RenderPassCollector(LogicalDevice* logicalDevice);
    ~RenderPassCollector() override;
    RenderPassCollector(RenderPassCollector&& other) noexcept;
    RenderPassCollector& operator=(RenderPassCollector&& other) noexcept;

 public:
    [[nodiscard]] std::shared_ptr<RenderPass> getOrCreateRenderPass(
        RenderTargetLayoutKey layoutKey);

    [[nodiscard]] std::shared_ptr<FrameBuffer> getOrCreateFrameBuffer(
        const std::shared_ptr<RenderPass>& renderPass,
        RenderTargetsInfo rtInfo);

    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    std::unordered_map<RenderTargetLayoutKey, std::shared_ptr<RenderPass>>
        _renderPassCollection;
    std::unordered_map<RenderTargetsInfo, std::shared_ptr<FrameBuffer>>
        _frameBufferCollection;
};
}  // namespace VoxFlow

#endif