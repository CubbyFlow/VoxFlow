// Author : snowapril

#include <VoxFlow/Core/Graphics/Commands/CommandConfig.hpp>
#include <VoxFlow/Core/Utils/HashUtil.hpp>

namespace VoxFlow
{
}  // namespace VoxFlow

std::size_t std::hash<VoxFlow::CommandStreamKey>::operator()(
    VoxFlow::CommandStreamKey const& streamKey) const noexcept
{
    uint32_t seed = 0;

    VoxFlow::hash_combine(seed, streamKey._cmdStreamName);
    return seed;
}