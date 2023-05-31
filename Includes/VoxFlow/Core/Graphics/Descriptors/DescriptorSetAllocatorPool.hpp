// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_ALLOCATOR_POOL_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_ALLOCATOR_POOL_HPP

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <unordered_map>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;
class DescriptorSetAllocator;
class ShaderModule;

class DescriptorSetAllocatorPool : private NonCopyable
{
 public:
    using ContainerType =
        std::unordered_map<DescriptorSetLayoutDesc,
                           std::shared_ptr<DescriptorSetAllocator>>;

 public:
    explicit DescriptorSetAllocatorPool(LogicalDevice* logicalDevice);
    ~DescriptorSetAllocatorPool();
    DescriptorSetAllocatorPool(DescriptorSetAllocatorPool&& rhs);
    DescriptorSetAllocatorPool& operator=(DescriptorSetAllocatorPool&& rhs);

 public:
    // Get or create new descriptor set allocator for given descriptor set
    // layout description
    std::shared_ptr<DescriptorSetAllocator> getOrCreateDescriptorSetAllocator(
        const DescriptorSetLayoutDesc& descSetLayout);

 private:
    LogicalDevice* _logicalDevice = nullptr;
    ContainerType _descriptorSetAllocators;
};

}  // namespace VoxFlow

#endif