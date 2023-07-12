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
    (void)frameGraph;

    spdlog::info("SceneObjectPass");
}

}  // namespace VoxFlow