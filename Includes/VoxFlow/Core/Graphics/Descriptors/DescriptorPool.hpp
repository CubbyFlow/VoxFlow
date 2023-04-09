// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_POOL_HPP
#define VOXEL_FLOW_DESCRIPTOR_POOL_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;
class Queue;
class CommandBuffer;

class DescriptorPool: private NonCopyable
{
 public:
    explicit DescriptorPool();
    ~DescriptorPool();
    DescriptorPool(DescriptorPool&& other) noexcept;
    DescriptorPool& operator=(DescriptorPool&& other) noexcept;
};
}  // namespace VoxFlow

#endif