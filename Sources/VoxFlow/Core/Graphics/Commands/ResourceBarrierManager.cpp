// Author : snowapril

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/ResourceBarrierManager.hpp>

namespace VoxFlow
{
void ResourceBarrierManager::addGlobalMemoryBarrier(
    ResourceAccessMask prevAccessMasks, ResourceAccessMask nextAccessMasks)
{
    (void)prevAccessMasks;
    (void)nextAccessMasks;
}

void ResourceBarrierManager::addMemoryBarrier(BindableResourceView* view,
                                              ResourceAccessMask accessMask)
{
    (void)view;
    (void)accessMask;
}

void ResourceBarrierManager::addExecutionBarrier(
    VkShaderStageFlagBits prevStageBits, VkShaderStageFlagBits nextStageBits)
{
    (void)prevStageBits;
    (void)nextStageBits;
}

void ResourceBarrierManager::commitPendingBarriers(
    VkCommandBuffer commandBuffer, const bool inRenderPassScope)
{
    const VkDependencyFlags dependencyFlag =
        inRenderPassScope ? VK_DEPENDENCY_BY_REGION_BIT : 0;

    if (_globalMemoryBarrier.isValid())
    {
        const VkMemoryBarrier memoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = _globalMemoryBarrier._srcAccessFlags,
            .dstAccessMask = _globalMemoryBarrier._dstAccessFlags
        };
        vkCmdPipelineBarrier(commandBuffer, _globalMemoryBarrier._srcStageFlags,
                             _globalMemoryBarrier._dstStageFlags,
                             dependencyFlag, 1, &memoryBarrier, 0, nullptr, 0,
                             nullptr);

        _globalMemoryBarrier.reset();
    }

    if (_executionBarrier.isValid())
    {
        vkCmdPipelineBarrier(commandBuffer, _executionBarrier._srcStageFlags,
                             _executionBarrier._dstStageFlags,
                             dependencyFlag, 0, nullptr, 0, nullptr, 0,
                             nullptr);

        _globalMemoryBarrier.reset();
    }

    if (_memoryBarrierGroup.isValid())
    {
        vkCmdPipelineBarrier(
            commandBuffer, _memoryBarrierGroup._srcStageFlags,
            _memoryBarrierGroup._dstStageFlags, dependencyFlag, 0, nullptr,
            static_cast<uint32>(_memoryBarrierGroup._bufferBarriers.size()),
            _memoryBarrierGroup._bufferBarriers.data(),
            static_cast<uint32>(_memoryBarrierGroup._imageBarriers.size()),
            _memoryBarrierGroup._imageBarriers.data());

        _memoryBarrierGroup.reset();
    }

}

}  // namespace VoxFlow