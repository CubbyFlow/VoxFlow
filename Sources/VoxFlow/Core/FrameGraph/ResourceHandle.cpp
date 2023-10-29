// Author : snowapril

#include <VoxFlow/Core/FrameGraph/ResourceHandle.hpp>

namespace VoxFlow
{

namespace RenderGraph
{
}  // namespace RenderGraph

}  // namespace VoxFlow

std::size_t std::hash<VoxFlow::RenderGraph::ResourceHandle>::operator()(VoxFlow::RenderGraph::ResourceHandle const& handle) const noexcept
{
    uint32_t seed = 0;
    VoxFlow::hash_combine(seed, handle.get());
    return seed;
}