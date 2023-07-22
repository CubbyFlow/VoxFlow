// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_CONFIG_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_CONFIG_HPP

#include <array>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>

namespace VoxFlow
{

enum class SetSlotCategory : uint8_t
{
    Bindless = 0,
    PerFrame = 1,
    PerRenderPass = 2,
    PerDraw = 3,
    Count = 4,
};

constexpr uint32_t MAX_NUM_SET_SLOTS =
    static_cast<uint32_t>(SetSlotCategory::Count);


constexpr std::array<uint32_t,
                     static_cast<uint32_t>(DescriptorCategory::Count)>
    NUM_BINDLESS_DESCRIPTORS = {
        100000,
        100000,
        100000,
    };

}  // namespace VoxFlow

#endif