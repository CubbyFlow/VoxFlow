// Author : snowapril

#ifndef VOXEL_FLOW_POST_PROCESS_PASS_HPP
#define VOXEL_FLOW_POST_PROCESS_PASS_HPP

#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <memory>

namespace VoxFlow
{
class RenderDevice;
class BasePipeline;
class Buffer;
class LogicalDevice;

namespace RenderGraph
{
class FrameGraph;
}

class PostProcessPass : public SceneRenderPass
{
 public:
    PostProcessPass(LogicalDevice* logicalDevice);
    ~PostProcessPass() override;

 public:
    bool initialize() override;
    void updateRender(ResourceUploadContext* uploadContext) override;
    void renderScene(RenderGraph::FrameGraph* frameGraph) override;

 protected:
    struct PostProcessPassData
    {
        uint32_t _renderPassID = 0;
    } _passData;

    std::unique_ptr<BasePipeline> _toneMapPipeline;
    LogicalDevice* _logicalDevice = nullptr;
};
}  // namespace VoxFlow

#endif