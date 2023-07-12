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
    (void)frameGraph;

    spdlog::info("PostProcessPas");
}

}  // namespace VoxFlow