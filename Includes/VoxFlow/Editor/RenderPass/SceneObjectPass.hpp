// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_OBJECT_PASS_HPP
#define VOXEL_FLOW_SCENE_OBJECT_PASS_HPP

#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class RenderDevice;
namespace FrameGraph
{
class FrameGraph;
}

class SceneObjectPass : public SceneRenderPass
{
 public:
    SceneObjectPass();
    ~SceneObjectPass() override;

 public:
    bool initialize() override;
    void renderScene(FrameGraph::FrameGraph* frameGraph) override;

 protected:
 private:
};
}  // namespace VoxFlow

#endif