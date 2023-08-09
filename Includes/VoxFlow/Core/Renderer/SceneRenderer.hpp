// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_RENDERER_HPP
#define VOXEL_FLOW_SCENE_RENDERER_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandConfig.hpp>
#include <string>
#include <taskflow/taskflow.hpp>
#include <unordered_map>

namespace VoxFlow
{
class LogicalDevice;
class SceneRenderPass;
class ResourceUploadContext;
class CommandJobSystem;
class SwapChain;

class SceneRenderer final : NonCopyable
{
 public:
    SceneRenderer(LogicalDevice* logicalDevice,
                  FrameGraph::FrameGraph* frameGraph,
                  CommandJobSystem* commandJobSystem);
    ~SceneRenderer() override;

 public:
    bool initializePasses(ResourceUploadContext* uploadContext);

    void beginFrameGraph(const FrameContext& frameContext);
    tf::Future<void> resolveSceneRenderPasses(SwapChain* swapChain);
    void submitFrameGraph();

    template <typename SceneRenderPassType, typename... Args,
              typename = typename std::enable_if_t<
                  std::is_base_of<SceneRenderPass, SceneRenderPassType>::value>>
    SceneRenderPassType* getOrCreateSceneRenderPass(const std::string& passName,
                                                Args... args)
    {
        SceneRenderPassType* newPassPtr =
            new SceneRenderPassType(args...);

        std::unique_ptr<SceneRenderPass> newPass(newPassPtr);

        _sceneRenderPasses.emplace(passName, std::move(newPass));
        return newPassPtr;
    }

 protected:
 private:
    LogicalDevice* _logicalDevice = nullptr;
    FrameGraph::FrameGraph* _frameGraph = nullptr;
    FrameContext _currentFrameContext;
    CommandJobSystem* _commandJobSystem = nullptr;
    CommandStreamKey _renderCmdStreamKey;

    std::unordered_map<std::string, std::unique_ptr<SceneRenderPass>>
        _sceneRenderPasses;
};
}  // namespace VoxFlow

#endif