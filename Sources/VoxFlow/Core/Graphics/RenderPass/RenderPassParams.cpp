// Author : snowapril

#include <VoxFlow/Core/Graphics/RenderPass/RenderPassParams.hpp>

namespace VoxFlow
{
}  // namespace VoxFlow

std::size_t std::hash<VoxFlow::RenderPassFlags>::operator()(VoxFlow::RenderPassFlags const& passFlags) const noexcept
{
    uint32_t seed = 0;

    VoxFlow::hash_combine(seed, static_cast<uint32_t>(passFlags._clearFlags));
    VoxFlow::hash_combine(seed, static_cast<uint32_t>(passFlags._loadFlags));
    VoxFlow::hash_combine(seed, static_cast<uint32_t>(passFlags._storeFlags));

    return seed;
}