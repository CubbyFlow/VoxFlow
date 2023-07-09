// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
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
}

}  // namespace VoxFlow