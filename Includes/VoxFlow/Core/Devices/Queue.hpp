// Author : snowapril

#ifndef VOXEL_FLOW_QUEUE_HPP
#define VOXEL_FLOW_QUEUE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <string>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;
class CommandBuffer;
class SwapChain;

class Queue : private NonCopyable
{
 public:
    explicit Queue(const std::string& debugName, LogicalDevice* logicalDevice, VkQueueFlags queueTypeFlags, VkQueue _queueTypeBits, uint32_t familyIndex,
                   uint32_t queueIndex) noexcept;
    ~Queue();
    Queue(Queue&& other) noexcept;
    Queue& operator=(Queue&& other) noexcept;

    [[nodiscard]] inline VkQueue get() const noexcept
    {
        return _queue;
    }

    [[nodiscard]] inline unsigned int getFamilyIndex() const noexcept
    {
        return _familyIndex;
    }

    [[nodiscard]] inline LogicalDevice* getLogicalDevice() const noexcept
    {
        return _logicalDevice;
    }

    inline const std::string& getDebugName() const
    {
        return _debugName;
    }

    inline VkQueueFlags getQueueFlags() const
    {
        return _queueTypeFlags;
    }

    // Submit given single command buffer to queue and returns FenceObject for
    // waiting submission completed
    FenceObject submitCommandBuffer(const std::shared_ptr<CommandBuffer>& commandBuffer, SwapChain* swapChain, const FrameContext* frameContext,
                                    const bool waitCompletion);

    // Submit given command buffers to queue and returns FenceObject for waiting
    // submission completed
    FenceObject submitCommandBufferBatch(std::vector<std::shared_ptr<CommandBuffer>>&& batchedCommandBuffers, SwapChain* swapChain,
                                         const FrameContext* frameContext, const bool waitAllCompletion);

    // Returns Timeline semaphore which synchronized with queue submission
    [[nodiscard]] inline VkSemaphore* getSubmitTimelineSemaphore()
    {
        return &_submitTimelineSemaphore;
    }

    // Returns last executed FenceValue of Timeline semaphore
    [[nodiscard]] inline uint64_t getLastExecutedFenceValue() const
    {
        return _lastExecutedFence.getFenceValue();
    }

    // Returns last signaled FenceValue of Timeline semaphore
    [[nodiscard]] inline uint64_t getLastCompletedFenceValue() const
    {
        return _lastCompletedFence.getFenceValue();
    }

    // Returns fence object that command buffer will use for synchronization
    // with this queue.
    [[nodiscard]] inline FenceObject allocateFenceToSignal()
    {
        _fenceToSignal.advanceFenceValue();
        return _fenceToSignal;
    }

    // Returns Timeline semaphore's value which synchronized with queue
    // submission
    [[nodiscard]] uint64_t querySemaphoreValue();

 private:
    std::string _debugName;
    LogicalDevice* _logicalDevice{ nullptr };
    VkQueueFlags _queueTypeFlags;
    VkQueue _queue{ VK_NULL_HANDLE };
    uint32_t _familyIndex{ 0 };
    uint32_t _queueIndex{ 0 };
    VkSemaphore _submitTimelineSemaphore{ VK_NULL_HANDLE };
    FenceObject _fenceToSignal = FenceObject::Default();
    FenceObject _lastExecutedFence = FenceObject::Default();
    FenceObject _lastCompletedFence = FenceObject::Default();
};
}  // namespace VoxFlow

#endif