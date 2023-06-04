// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_ALLOCATOR_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_ALLOCATOR_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>

namespace VoxFlow
{
class LogicalDevice;
class Queue;
class CommandBuffer;

class DescriptorSetAllocator : private NonCopyable
{
 public:
    explicit DescriptorSetAllocator(LogicalDevice* logicalDevice,
                                    const DescriptorSetLayoutDesc& setLayout);
    ~DescriptorSetAllocator();
    DescriptorSetAllocator(DescriptorSetAllocator&& other) noexcept;
    DescriptorSetAllocator& operator=(DescriptorSetAllocator&& other) noexcept;

    [[nodiscard]] inline VkDescriptorSetLayout getVkDescriptorSetLayout() const
    {
        return _vkSetLayout;
    }

    // Get or create pooled descriptor set with predefined descriptor set layout
    // binding infos. Allocated descriptor set will be reused when the given
    // fence object is completed.
    [[nodiscard]] VkDescriptorSet getOrCreatePooledDescriptorSet(
        const FenceObject& fenceObject);

    // Allocate bindless descriptor set which will be used forever.
    [[nodiscard]] VkDescriptorSet allocateBindlessDescriptorSet();

    // Get descriptor set layout description for this allocator
    [[nodiscard]] inline DescriptorSetLayoutDesc getDescriptorSetLayoutDesc()
        const
    {
        return _setLayoutDesc;
    }

 private:
    void release();

 private:
    // TODO(snowapril) : allow arbitrary number of descriptor sets.
    static const uint32_t MAX_NUM_DESC_SETS = 16;

    LogicalDevice* _logicalDevice = nullptr;
    DescriptorSetLayoutDesc _setLayoutDesc;
    VkDescriptorSetLayout _vkSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool _vkDescPool = VK_NULL_HANDLE;

    struct PooledDescriptorSetNode
    {
        VkDescriptorSet _vkPooledDescriptorSet = VK_NULL_HANDLE;
        FenceObject _lastAccessedFenceObject = FenceObject::Default();
    };

    std::vector<PooledDescriptorSetNode> _pooledDescriptorSetNodes;
};
}  // namespace VoxFlow

#endif