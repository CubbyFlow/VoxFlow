// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_OBJECT_PASS_HPP
#define VOXEL_FLOW_SCENE_OBJECT_PASS_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Scene/SceneObjectCollection.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>

namespace VoxFlow
{
class RenderDevice;
class Buffer;
class GraphicsPipeline;
class RenderDevice;

namespace RenderGraph
{
class FrameGraph;
}

class SceneObjectPass : public SceneRenderPass
{
 public:
    SceneObjectPass(RenderDevice* renderDevice);
    ~SceneObjectPass() override;

 public:
    bool initialize() override;
    void updateRender(ResourceUploadContext* uploadContext) override;
    void renderScene(RenderGraph::FrameGraph* frameGraph) override;

 protected:
 private:
    struct SceneObjectPassData
    {
        RenderGraph::ResourceHandle _sceneColorHandle;
        RenderGraph::ResourceHandle _sceneDepthHandle;
        uint32_t _renderPassID = UINT32_MAX;
    } _passData;

    std::shared_ptr<GraphicsPipeline> _sceneObjectPipeline;
    std::unique_ptr<Buffer> _cubeVertexBuffer;
    std::unique_ptr<Buffer> _cubeIndexBuffer;
    std::unique_ptr<SceneObjectCollection> _sceneObjectCollection;
    RenderDevice* _renderDevice = nullptr;
};
}  // namespace VoxFlow

#endif