// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_PASS_COLLECTOR_HPP
#define VOXEL_FLOW_RENDER_PASS_COLLECTOR_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Utils/RendererCommon-Impl.hpp>
#include <memory>
#include <unordered_map>

namespace VoxFlow
{
class LogicalDevice;
class RenderPass;
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

    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    std::unordered_map<RenderTargetLayoutKey, std::shared_ptr<RenderPass>>
        _renderPassCollection;
};
}  // namespace VoxFlow

#endif