// Author : snowapril

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>

namespace VoxFlow
{

}  // namespace VoxFlow

std::size_t std::hash<VoxFlow::DescriptorInfo>::operator()(
    VoxFlow::DescriptorInfo const& info) const noexcept
{
    uint32_t seed = 0;

    VoxFlow::hash_combine(seed, static_cast<uint32_t>(info._setCategory));
    VoxFlow::hash_combine(seed, static_cast<uint32_t>(info._descriptorCategory));
    VoxFlow::hash_combine(seed, info._arraySize);
    VoxFlow::hash_combine(seed, info._binding);

    return seed;
}

std::size_t std::hash<VoxFlow::DescriptorSetLayoutDesc>::operator()(
    VoxFlow::DescriptorSetLayoutDesc const& setLayout) const noexcept
{
    uint32_t seed = 0;

    for (const VoxFlow::DescriptorInfo& info : setLayout._descriptorInfos)
    {
        VoxFlow::hash_combine(seed, info);
    }

    VoxFlow::hash_combine(seed, static_cast<uint32_t>(setLayout._stageFlags));
    return seed;
}