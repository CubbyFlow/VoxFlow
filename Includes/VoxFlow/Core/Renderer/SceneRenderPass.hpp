// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_RENDER_PASS_HPP
#define VOXEL_FLOW_SCENE_RENDER_PASS_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <string>
#include <vector>

namespace VoxFlow
{
namespace RenderGraph
{
class FrameGraph;
}

class ResourceUploadContext;

class SceneRenderPass : NonCopyable
{
 public:
    SceneRenderPass();
    virtual ~SceneRenderPass(){};

    /**
     * @brief Given render pass must precede this pass
     * @param passName name of the scene render pass that this pass will be
     * dependent on
     * @return self
     */
    SceneRenderPass* addDependency(const std::string& passName);

    /**
     * @return get added dependency pass names
     */
    inline const std::vector<std::string>* getDepenentPasses() const
    {
        return &_dependencyPass;
    }

 public:
    virtual bool initialize() = 0;
    virtual void updateRender(ResourceUploadContext* uploadContext) = 0;
    virtual void renderScene(RenderGraph::FrameGraph* frameGraph) = 0;

 protected:
    std::vector<std::string> _dependencyPass;
};
}  // namespace VoxFlow

#endif