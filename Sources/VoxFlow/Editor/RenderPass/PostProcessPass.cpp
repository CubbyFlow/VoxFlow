// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Editor/RenderPass/PostProcessPass.hpp>

namespace VoxFlow
{

PostProcessPass::PostProcessPass()
{
}

PostProcessPass::~PostProcessPass()
{
}

bool PostProcessPass::initialize()
{
    return true;
}

void PostProcessPass::renderScene(FrameGraph::FrameGraph* frameGraph)
{
    FrameGraph::ResourceHandle backBufferHandle =
        frameGraph->getBlackBoard().getHandle("BackBuffer");

    struct TempPassData
    {
    };

    frameGraph->addCallbackPass<TempPassData>(
        "PostProcessPass",
        [&](FrameGraph::FrameGraphBuilder& builder, TempPassData&) {
            builder.write(backBufferHandle);
        },
        [&](FrameGraph::FrameGraph*, TempPassData&, CommandJobSystem*) {
            spdlog::info("PostProcesPass");
        });
}

}  // namespace VoxFlow