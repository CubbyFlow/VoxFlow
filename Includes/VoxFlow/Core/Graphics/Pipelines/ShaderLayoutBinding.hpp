// Author : snowapril

#ifndef VOXEL_FLOW_SHADER_LAYOUT_BINDING_HPP
#define VOXEL_FLOW_SHADER_LAYOUT_BINDING_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/HashUtil.hpp>
#include <functional>
#include <string>

template <>
struct std::hash<VkDescriptorSetLayoutBinding>
{
    std::size_t operator()(
        VkDescriptorSetLayoutBinding const& layoutBinding) const noexcept
    {
        uint32_t seed = 0;
        VoxFlow::hash_combine(seed, layoutBinding.binding);
        VoxFlow::hash_combine(seed, static_cast<uint32_t>(layoutBinding.descriptorType));
        VoxFlow::hash_combine(seed, layoutBinding.descriptorCount);
        VoxFlow::hash_combine(seed, static_cast<uint32_t>(layoutBinding.stageFlags));
        VoxFlow::hash_combine(
            seed, reinterpret_cast<uint64_t>(layoutBinding.pImmutableSamplers));
        return seed;
    }
};

namespace VoxFlow
{
struct ShaderLayoutBinding
{
    std::string _resourceName;
    uint32_t _set = 0;
    VkDescriptorSetLayoutBinding _vkLayoutBindnig = {};
    uint32_t _hashCached = 0;

    ShaderLayoutBinding(const std::string& resourceName, const uint32_t set,
                        const VkDescriptorSetLayoutBinding& vkLayoutBinding)
        : _resourceName(resourceName),
          _set(set),
          _vkLayoutBindnig(vkLayoutBinding)
    {
        hash_combine(_hashCached, _resourceName);
        hash_combine(_hashCached, _set);
        hash_combine(_hashCached, _vkLayoutBindnig);
    }
};

}  // namespace VoxFlow

#endif