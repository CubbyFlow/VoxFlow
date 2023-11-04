// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Utils/MathUtils.hpp>

namespace VoxFlow
{
Queue::Queue(const std::string& debugName, LogicalDevice* logicalDevice, VkQueueFlags queueTypeFlags, VkQueue queueHandle, uint32_t familyIndex,
             uint32_t queueIndex) noexcept
    : _debugName(debugName),
      _logicalDevice(logicalDevice),
      _queueTypeFlags(queueTypeFlags),
      _queue(queueHandle),
      _familyIndex(familyIndex),
      _queueIndex(queueIndex),
      _fenceToSignal(this, 0ULL),
      _lastExecutedFence(this, 0ULL),
      _lastCompletedFence(this, 0ULL)
{
#if defined(VK_DEBUG_NAME_ENABLED)
    DebugUtil::setObjectName(_logicalDevice, queueHandle, _debugName.c_str());
#endif

    VkSemaphoreTypeCreateInfo timelineCreateInfo;
    timelineCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
    timelineCreateInfo.pNext = NULL;
    timelineCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
    timelineCreateInfo.initialValue = 0;

    VkSemaphoreCreateInfo createInfo;
    createInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
    createInfo.pNext = &timelineCreateInfo;
    createInfo.flags = 0;

    vkCreateSemaphore(_logicalDevice->get(), &createInfo, NULL, &_submitTimelineSemaphore);

#if defined(VK_DEBUG_NAME_ENABLED)
    std::string timelineSemaphoreDebugName = fmt::format("{}_TimelineSemaphore", _debugName);
    DebugUtil::setObjectName(_logicalDevice, _submitTimelineSemaphore, timelineSemaphoreDebugName.c_str());
#endif
}

Queue::~Queue()
{
    vkQueueWaitIdle(_queue);
    vkDestroySemaphore(_logicalDevice->get(), _submitTimelineSemaphore, nullptr);
}

Queue::Queue(Queue&& other) noexcept
{
    operator=(std::move(other));
}

Queue& Queue::operator=(Queue&& other) noexcept
{
    if (this != &other)
    {
        _queue = other._queue;
        _familyIndex = other._familyIndex;
        _queueIndex = other._queueIndex;
        _logicalDevice = other._logicalDevice;
        _submitTimelineSemaphore = other._submitTimelineSemaphore;

        other._queue = VK_NULL_HANDLE;
        other._submitTimelineSemaphore = VK_NULL_HANDLE;
    }
    return *this;
}

FenceObject Queue::submitCommandBuffer(const std::shared_ptr<CommandBuffer>& commandBuffer, SwapChain* swapChain, const FrameContext* frameContext,
                                       const bool waitCompletion)
{
    VOX_ASSERT((swapChain == nullptr) || (frameContext == nullptr) || (frameContext->_frameIndex < FRAME_BUFFER_COUNT),
               "Must provide valid frame index when swapChain is not nullptr");

    VkCommandBuffer cmdBufferToSubmit = commandBuffer->get();

    uint64_t signalingValues[2] = { commandBuffer->getFenceToSignal().getFenceValue(), 0ULL };

    uint64_t waitingValues[2] = { signalingValues[0] - 1, 0ULL };
    VkTimelineSemaphoreSubmitInfo timelineInfo{
        .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreValueCount = swapChain == nullptr ? 1U : 2U,  // TODO
        .pWaitSemaphoreValues = waitingValues,                      // TODO
        .signalSemaphoreValueCount = swapChain == nullptr ? 1U : 2U,
        .pSignalSemaphoreValues = signalingValues,
    };

    VkSemaphore waitingSemaphores[2] = { _submitTimelineSemaphore, swapChain == nullptr ? VK_NULL_HANDLE : swapChain->getCurrentBackBufferReadySemaphore() };

    VkSemaphore signalingSemaphores[2] = { _submitTimelineSemaphore, swapChain == nullptr ? VK_NULL_HANDLE : swapChain->getCurrentPresentReadySemaphore() };

    // TODO(snowapril) : modify stage masks
    VkPipelineStageFlags waitDstStageMasks[2] = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };

    // Add wait semaphore for waiting acquire back buffer image
    VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = &timelineInfo,
                                .waitSemaphoreCount = swapChain != nullptr ? 2U : 1U,
                                .pWaitSemaphores = &waitingSemaphores[0],
                                .pWaitDstStageMask = waitDstStageMasks,
                                .commandBufferCount = 1,
                                .pCommandBuffers = &cmdBufferToSubmit,
                                .signalSemaphoreCount = swapChain != nullptr ? 2U : 1U,
                                .pSignalSemaphores = &signalingSemaphores[0] };

    _lastExecutedFence = FenceObject(this, waitingValues[0]);

    VK_ASSERT(vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE));

    _lastExecutedFence.advanceFenceValue();

    if (waitCompletion)
    {
        VkSemaphoreWaitInfo waitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .pNext = NULL,
            .flags = 0,
            .semaphoreCount = 1,
            .pSemaphores = &_submitTimelineSemaphore,
            .pValues = signalingValues,
        };

        vkWaitSemaphoresKHR(_logicalDevice->get(), &waitInfo, UINT64_MAX);
        _lastCompletedFence = FenceObject(this, signalingValues[0]);
    }
    else if (swapChain != nullptr && frameContext != nullptr)
    {
        swapChain->addWaitSemaphores(frameContext->_frameIndex, _submitTimelineSemaphore, signalingValues[0]);
    }

    return _lastExecutedFence;
}

FenceObject Queue::submitCommandBufferBatch(std::vector<std::shared_ptr<CommandBuffer>>&& batchedCommandBuffers, SwapChain* swapChain,
                                            const FrameContext* frameContext, const bool waitAllCompletion)
{
    VOX_ASSERT((swapChain == nullptr) || (frameContext == nullptr) || (frameContext->_frameIndex < FRAME_BUFFER_COUNT),
               "Must provide valid frame index when swapChain is not nullptr");

    std::vector<std::shared_ptr<CommandBuffer>>&& commandBuffersToSubmit = std::move(batchedCommandBuffers);

    // Must sort given command buffers with fence value allocated to
    // guarantee sequential execution.
    std::sort(commandBuffersToSubmit.begin(), commandBuffersToSubmit.end(),
              [](const std::shared_ptr<CommandBuffer>& lhs, const std::shared_ptr<CommandBuffer>& rhs) {
                  return lhs->getFenceToSignal().getFenceValue() < rhs->getFenceToSignal().getFenceValue();
              });

    std::vector<VkCommandBuffer> cmdBuffersToSubmit;
    cmdBuffersToSubmit.reserve(batchedCommandBuffers.size());

    uint64_t maxFenceSignalValue = 0;
    std::transform(batchedCommandBuffers.begin(), batchedCommandBuffers.end(), std::back_inserter(cmdBuffersToSubmit),
                   [&maxFenceSignalValue](const std::shared_ptr<CommandBuffer>& cmdBuffer) {
                       maxFenceSignalValue = glm::max(maxFenceSignalValue, cmdBuffer->getFenceToSignal().getFenceValue());
                       return cmdBuffer->get();
                   });

    uint64_t signalingValues[2] = { maxFenceSignalValue, 0ULL };

    uint64_t waitingValues[2] = { signalingValues[0] - 1, 0ULL };
    VkTimelineSemaphoreSubmitInfo timelineInfo{
        .sType = VK_STRUCTURE_TYPE_TIMELINE_SEMAPHORE_SUBMIT_INFO,
        .pNext = NULL,
        .waitSemaphoreValueCount = swapChain == nullptr ? 1U : 2U,  // TODO
        .pWaitSemaphoreValues = waitingValues,                      // TODO
        .signalSemaphoreValueCount = swapChain == nullptr ? 1U : 2U,
        .pSignalSemaphoreValues = signalingValues,
    };

    VkSemaphore waitingSemaphores[2] = { _submitTimelineSemaphore, swapChain == nullptr ? VK_NULL_HANDLE : swapChain->getCurrentBackBufferReadySemaphore() };

    VkSemaphore signalingSemaphores[2] = { _submitTimelineSemaphore, swapChain == nullptr ? VK_NULL_HANDLE : swapChain->getCurrentPresentReadySemaphore() };

    // TODO(snowapril) : modify stage masks
    VkPipelineStageFlags waitDstStageMasks[2] = { VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT };

    // Add wait semaphore for waiting acquire back buffer image
    VkSubmitInfo submitInfo = { .sType = VK_STRUCTURE_TYPE_SUBMIT_INFO,
                                .pNext = &timelineInfo,
                                .waitSemaphoreCount = swapChain != nullptr ? 2U : 1U,
                                .pWaitSemaphores = &waitingSemaphores[0],
                                .pWaitDstStageMask = waitDstStageMasks,
                                .commandBufferCount = static_cast<uint32_t>(cmdBuffersToSubmit.size()),
                                .pCommandBuffers = cmdBuffersToSubmit.data(),
                                .signalSemaphoreCount = swapChain != nullptr ? 2U : 1U,
                                .pSignalSemaphores = &signalingSemaphores[0] };

    _lastExecutedFence = FenceObject(this, waitingValues[0]);

    VK_ASSERT(vkQueueSubmit(_queue, 1, &submitInfo, VK_NULL_HANDLE));

    _lastExecutedFence.advanceFenceValue();

    if (waitAllCompletion)
    {
        VkSemaphoreWaitInfo waitInfo{
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO,
            .pNext = NULL,
            .flags = 0,
            .semaphoreCount = 1,
            .pSemaphores = &_submitTimelineSemaphore,
            .pValues = signalingValues,
        };

        vkWaitSemaphoresKHR(_logicalDevice->get(), &waitInfo, UINT64_MAX);
        _lastCompletedFence = FenceObject(this, maxFenceSignalValue);
    }
    else if (swapChain != nullptr && frameContext != nullptr)
    {
        swapChain->addWaitSemaphores(frameContext->_frameIndex, _submitTimelineSemaphore, maxFenceSignalValue);
    }

    return _lastExecutedFence;
}

uint64_t Queue::querySemaphoreValue()
{
    uint64_t value = 0;
    VK_ASSERT(vkGetSemaphoreCounterValueKHR(_logicalDevice->get(), _submitTimelineSemaphore, &value));

    if (value == UINT64_MAX)
    {
        // As returning UINT64_MAX means current logical device is lost,
        // call its handler.
        DeviceRemoveTracker::get()->onDeviceRemoved();
    }

    _lastCompletedFence = FenceObject(this, value);
    return value;
}

}  // namespace VoxFlow