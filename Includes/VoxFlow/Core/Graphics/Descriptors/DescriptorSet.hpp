// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;
class Queue;
class CommandBuffer;

class DescriptorSet : private NonCopyable
{
 public:
    explicit DescriptorSet();
    ~DescriptorSet();
    DescriptorSet(DescriptorSet&& other) noexcept;
    DescriptorSet& operator=(DescriptorSet&& other) noexcept;
};
}  // namespace VoxFlow

#endif