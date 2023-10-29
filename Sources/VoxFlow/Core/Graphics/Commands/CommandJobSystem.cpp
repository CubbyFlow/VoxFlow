// Author : snowapril

#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandPool.hpp>

namespace VoxFlow
{
CommandStream::CommandStream(LogicalDevice* logicalDevice, Queue* queue) : _logicalDevice(logicalDevice), _queue(queue)
{
}

CommandStream::~CommandStream()
{
}

FenceObject CommandStream::flush(SwapChain* swapChain, const FrameContext* frameContext, const bool waitAllCompletion)
{
    std::vector<std::shared_ptr<CommandBuffer>> cmdBufs;
    {
        std::lock_guard<std::recursive_mutex> scopedLock(_streamMutex);
        for (auto& [_, cmdBufPtr] : _cmdBufferStorage)
        {
            cmdBufPtr->endCommandBuffer();
            cmdBufs.emplace_back(std::move(cmdBufPtr));
        }
        _cmdBufferStorage.clear();
    }

    if (cmdBufs.empty())
    {
        return FenceObject::Default();
    }

    // TODO(snowapril) : sort command buffer according and set dependency
    FenceObject fenceToSignal = _queue->submitCommandBufferBatch(std::move(cmdBufs), swapChain, frameContext, waitAllCompletion);

    return fenceToSignal;
}

CommandBuffer* CommandStream::getOrAllocateCommandBuffer()
{
    const auto threadId = std::this_thread::get_id();

    CommandBuffer* cmdBuffer = nullptr;
    {
        std::lock_guard<std::recursive_mutex> scopedLock(_streamMutex);

        auto cmdIter = _cmdBufferStorage.find(threadId);
        if (cmdIter == _cmdBufferStorage.end())
        {
            CommandPool* cmdPool = getOrAllocateCommandPool();
            std::shared_ptr<CommandBuffer> cmdBufferPtr = cmdPool->getOrCreateCommandBuffer();
            // TODO(snowapril) : add thread_id to command buffer begin name
            cmdBufferPtr->beginCommandBuffer(_queue->allocateFenceToSignal(), fmt::format("CommandStream"));

            cmdBuffer = cmdBufferPtr.get();
            _cmdBufferStorage.emplace(threadId, std::move(cmdBufferPtr));
        }
        else
        {
            cmdBuffer = cmdIter->second.get();
        }
    }

    return cmdBuffer;
}

CommandPool* CommandStream::getOrAllocateCommandPool()
{
    const auto threadId = std::this_thread::get_id();

    CommandPool* cmdPool = nullptr;
    {
        std::lock_guard<std::recursive_mutex> scopedLock(_streamMutex);
        auto cmdPoolIter = _cmdPoolStorage.find(threadId);
        if (cmdPoolIter == _cmdPoolStorage.end())
        {
            auto cmdPoolPtr = std::make_unique<CommandPool>(_logicalDevice, _queue);
            cmdPool = cmdPoolPtr.get();
            _cmdPoolStorage.emplace(threadId, std::move(cmdPoolPtr));
        }
        else
        {
            cmdPool = cmdPoolIter->second.get();
        }
    }

    return cmdPool;
}

CommandJobSystem::CommandJobSystem(LogicalDevice* logicalDevice) : _logicalDevice(logicalDevice)
{
}

CommandJobSystem::~CommandJobSystem()
{
}

void CommandJobSystem::createCommandStream(const CommandStreamKey& streamKey, Queue* queue)
{
    _cmdStreams.emplace(streamKey, std::make_unique<CommandStream>(_logicalDevice, queue));
}

CommandStream* CommandJobSystem::getCommandStream(const CommandStreamKey& streamKey)
{
    CommandStream* cmdStream = nullptr;
    if (auto cmdStreamIter = _cmdStreams.find(streamKey); cmdStreamIter != _cmdStreams.end())
    {
        cmdStream = cmdStreamIter->second.get();
    }
    return cmdStream;
}

}  // namespace VoxFlow
