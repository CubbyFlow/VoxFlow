// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_OBJECT_PASS_HPP
#define VOXEL_FLOW_SCENE_OBJECT_PASS_HPP

#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>

namespace VoxFlow
{
class RenderDevice;
class Buffer;
class GraphicsPipeline;
class LogicalDevice;

namespace FrameGraph
{
class FrameGraph;
}

class SceneObjectPass : public SceneRenderPass
{
 public:
    SceneObjectPass(LogicalDevice* logicalDevice);
    ~SceneObjectPass() override;

 public:
    bool initialize() override;
    void updateRender(ResourceUploadContext* uploadContext);
    void renderScene(FrameGraph::FrameGraph* frameGraph) override;

 protected:
 private:
    std::unique_ptr<GraphicsPipeline> _sceneObjectPipeline;
    std::unique_ptr<Buffer> _cubeVertexBuffer;
    std::unique_ptr<Buffer> _cubeIndexBuffer;
    LogicalDevice* _logicalDevice = nullptr;
};
}  // namespace VoxFlow

#endif