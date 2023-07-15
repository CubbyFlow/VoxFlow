// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Editor/RenderPass/SceneObjectPass.hpp>

namespace VoxFlow
{

SceneObjectPass::SceneObjectPass()
{
}

SceneObjectPass::~SceneObjectPass()
{
}

bool SceneObjectPass::initialize()
{
    return true;
}

void SceneObjectPass::renderScene(FrameGraph::FrameGraph* frameGraph)
{
    FrameGraph::ResourceHandle backBufferHandle =
        frameGraph->getBlackBoard().getHandle("BackBuffer");

    struct TempPassData
    {
    };

    frameGraph->addCallbackPass<TempPassData>(
        "SceneObjectPass",
        [&](FrameGraph::FrameGraphBuilder& builder, TempPassData&) {
            builder.write(backBufferHandle);
        },
        [&](FrameGraph::FrameGraph*, TempPassData&, CommandExecutorBase*) {
            spdlog::info("SceneObjectPass");
        });
}

}  // namespace VoxFlow