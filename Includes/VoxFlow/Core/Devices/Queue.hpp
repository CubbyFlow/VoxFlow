// Author : snowapril

#ifndef VOXEL_FLOW_QUEUE_HPP
#define VOXEL_FLOW_QUEUE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
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
    explicit Queue(const std::string& debugName, LogicalDevice* logicalDevice, VkQueueFlags queueTypeFlags,
                   VkQueue _queueTypeBits, uint32_t familyIndex,
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

    // Submit given single command buffer to queue and returns FenceObject for
    // waiting submission completed
    FenceObject submitCommandBuffer(
        const std::shared_ptr<CommandBuffer>& commandBuffer,
        const std::shared_ptr<SwapChain>& swapChain, const uint32_t frameIndex,
        const bool waitCompletion);

    // Submit given command buffers to queue and returns FenceObject for waiting
    // submission completed
    FenceObject submitCommandBufferBatch(
        const std::vector<std::shared_ptr<CommandBuffer>>&
            batchedCommandBuffers,
        const std::shared_ptr<SwapChain>& swapChain, const uint32_t frameIndex,
        const bool waitAllCompletion);

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

    // Returns Timeline semaphore's value which synchronized with queue
    // submission
    [[nodiscard]] uint64_t getTimelineSemaphoreValue();

 private:
    std::string _debugName;
    LogicalDevice* _logicalDevice{ nullptr };
    VkQueueFlags _queueTypeFlags;
    VkQueue _queue{ VK_NULL_HANDLE };
    uint32_t _familyIndex{ 0 };
    uint32_t _queueIndex{ 0 };
    VkSemaphore _submitTimelineSemaphore{ VK_NULL_HANDLE };
    FenceObject _lastExecutedFence;
    FenceObject _lastCompletedFence;
};
}  // namespace VoxFlow

#endif