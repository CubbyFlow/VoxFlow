// Author : snowapril

#ifndef VOXEL_FLOW_POST_PROCESS_PASS_HPP
#define VOXEL_FLOW_POST_PROCESS_PASS_HPP

#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class RenderDevice;
namespace FrameGraph
{
class FrameGraph;
}

class PostProcessPass : public SceneRenderPass
{
 public:
    PostProcessPass();
    ~PostProcessPass() override;

 public:
    bool initialize() override;
    void renderScene(FrameGraph::FrameGraph* frameGraph) override;

 protected:
 private:
};
}  // namespace VoxFlow

#endif