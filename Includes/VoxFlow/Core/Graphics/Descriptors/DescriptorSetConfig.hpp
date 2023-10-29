// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_CONFIG_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_CONFIG_HPP

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <array>

namespace VoxFlow
{
constexpr uint32_t MAX_NUM_SET_SLOTS = static_cast<uint32_t>(SetSlotCategory::Count);

constexpr std::array<uint32_t, static_cast<uint32_t>(DescriptorCategory::Count)> NUM_BINDLESS_DESCRIPTORS = {
    100000,
    100000,
    100000,
};

}  // namespace VoxFlow

#endif