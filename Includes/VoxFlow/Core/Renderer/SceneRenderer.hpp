// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_RENDERER_HPP
#define VOXEL_FLOW_SCENE_RENDERER_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <string>
#include <unordered_map>

namespace VoxFlow
{
class LogicalDevice;
class SceneRenderPass;

class SceneRenderer : NonCopyable
{
 public:
    SceneRenderer(LogicalDevice* logicalDevice,
                  FrameGraph::FrameGraph* frameGraph);
    ~SceneRenderer();

 public:
    bool initialize();

    void beginFrameGraph(const FrameContext& frameContext);
    void renderScene();

    template <typename SceneRenderPassType, typename... Args,
              typename = typename std::enable_if_t<
                  std::is_base_of<SceneRenderPass, SceneRenderPassType>::value>>
    SceneRenderPass* getOrCreateSceneRenderPass(const std::string& passName,
                                                Args... args)
    {
        std::unique_ptr<SceneRenderPass> newPass =
            std::make_unique<SceneRenderPassType>(args...);
        SceneRenderPass* newPassPtr = newPass.get();
        _sceneRenderPasses.emplace(passName, std::move(newPass));
        return newPassPtr;
    }
    
 protected:
 private:
    LogicalDevice* _logicalDevice = nullptr;
    FrameGraph::FrameGraph* _frameGraph = nullptr;
    FrameContext _currentFrameContext;

    std::unordered_map<std::string, std::unique_ptr<SceneRenderPass>>
        _sceneRenderPasses;
};
}  // namespace VoxFlow

#endif