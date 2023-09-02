// Author : snowapril

#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>

namespace VoxFlow
{

SceneRenderPass::SceneRenderPass()
{
}

SceneRenderPass* SceneRenderPass::addDependency(const std::string& passName)
{
    _dependencyPass.emplace_back(passName);
    return this;
}

}  // namespace VoxFlow