// Author : snowapril

#ifndef VOXEL_FLOW_DESCRIPTOR_SET_ALLOCATOR_HPP
#define VOXEL_FLOW_DESCRIPTOR_SET_ALLOCATOR_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;
class Queue;
class CommandBuffer;

class DescriptorSetAllocator : private NonCopyable
{
 protected:
    explicit DescriptorSetAllocator(LogicalDevice* logicalDevice,
                                    const bool isBindless);

 public:
    ~DescriptorSetAllocator() override;
    DescriptorSetAllocator(DescriptorSetAllocator&& other) noexcept;
    DescriptorSetAllocator& operator=(DescriptorSetAllocator&& other) noexcept;

    [[nodiscard]] inline VkDescriptorSetLayout getVkDescriptorSetLayout() const
    {
        return _vkSetLayout;
    }

    // Get descriptor set layout description for this allocator
    [[nodiscard]] inline DescriptorSetLayoutDesc getDescriptorSetLayoutDesc()
        const
    {
        return _setLayoutDesc;
    }

    bool initialize(const DescriptorSetLayoutDesc& setLayout,
                    const uint32_t numSets);

 private:
    void release();

 protected:
    struct DescriptorSetNode
    {
        VkDescriptorSet _vkDescriptorSet = VK_NULL_HANDLE;
        FenceObject _lastAccessedFenceObject = FenceObject::Default();
    };

    LogicalDevice* _logicalDevice = nullptr;
    DescriptorSetLayoutDesc _setLayoutDesc;
    VkDescriptorSetLayout _vkSetLayout = VK_NULL_HANDLE;
    VkDescriptorPool _vkDescPool = VK_NULL_HANDLE;

    std::vector<DescriptorSetNode> _descriptorSetNodes;
    bool _isBindless = false;
};

class PooledDescriptorSetAllocator final : public DescriptorSetAllocator
{
 public:
    explicit PooledDescriptorSetAllocator(LogicalDevice* logicalDevice);
    ~PooledDescriptorSetAllocator() override;
    PooledDescriptorSetAllocator(PooledDescriptorSetAllocator&& other) noexcept;
    PooledDescriptorSetAllocator& operator=(
        PooledDescriptorSetAllocator&& other) noexcept;

    // Get or create pooled descriptor set with predefined descriptor set layout
    // binding infos. Allocated descriptor set will be reused when the given
    // fence object is completed.
    [[nodiscard]] VkDescriptorSet getOrCreatePooledDescriptorSet(
        const FenceObject& fenceObject);
};

class BindlessDescriptorSetAllocator final : public DescriptorSetAllocator
{
 public:
    explicit BindlessDescriptorSetAllocator(LogicalDevice* logicalDevice);
    ~BindlessDescriptorSetAllocator() override;
    BindlessDescriptorSetAllocator(
        BindlessDescriptorSetAllocator&& other) noexcept;
    BindlessDescriptorSetAllocator& operator=(
        BindlessDescriptorSetAllocator&& other) noexcept;

    // Allocate bindless descriptor set which will be used forever.
    [[nodiscard]] VkDescriptorSet getBindlessDescriptorSet(
        const uint32_t setIndex, const FenceObject& fenceObject);
};
}  // namespace VoxFlow

#endif