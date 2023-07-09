// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_CONFIG_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_CONFIG_HPP

#include <array>

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

enum class BindlessDescriptorBinding
{
    CombinedImage = 0,
    UniformBuffer = 1,
    StorageBuffer = 2,
    Unknown = 3,
    Count = Unknown,
};

constexpr std::array<uint32_t,
                     static_cast<uint32_t>(BindlessDescriptorBinding::Count)>
    NUM_BINDLESS_DESCRIPTORS = {
        100000,
        100000,
        100000,
    };

}  // namespace VoxFlow

#endif