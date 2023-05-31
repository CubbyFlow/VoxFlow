// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandPool.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include <thread>

namespace VoxFlow
{
CommandPool::CommandPool(LogicalDevice* logicalDevice, Queue* ownerQueue)
    : _creationThreadId(std::this_thread::get_id()),
      _logicalDevice(logicalDevice),
      _ownerQueue(ownerQueue)

{
    VkCommandPoolCreateInfo poolInfo{
        .sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT,
        .queueFamilyIndex = _ownerQueue->getFamilyIndex(),
    };
    VK_ASSERT(vkCreateCommandPool(_logicalDevice->get(), &poolInfo, nullptr,
                                  &_commandPool));

#if defined(VK_DEBUG_NAME_ENABLED)
    const std::string debugName =
        _ownerQueue->getDebugName() + "_" +
        std::to_string(reinterpret_cast<uint64_t>(_commandPool));

    if (_commandPool != VK_NULL_HANDLE)
    {
        DebugUtil::setObjectName(_logicalDevice, _commandPool,
                                 debugName.c_str());
    }
    else
#endif
    {
        VOX_ASSERT(false, "Failed to create CommandPool");
    }
}

CommandPool ::~CommandPool()
{
    if (_commandPool != VK_NULL_HANDLE)
        vkDestroyCommandPool(_logicalDevice->get(), _commandPool, nullptr);
}

CommandPool::CommandPool(CommandPool&& other) noexcept
{
    operator=(std::move(other));
}

CommandPool& CommandPool::operator=(CommandPool&& other) noexcept
{
    VOX_ASSERT(_creationThreadId == other._creationThreadId, "");
    if (this != &other)
    {
        _creationThreadId = other._creationThreadId;
        _logicalDevice = other._logicalDevice;
        _ownerQueue = other._ownerQueue;
        _commandPool = other._commandPool;
        _freedCommandBuffers.swap(other._freedCommandBuffers);

        other._commandPool = VK_NULL_HANDLE;
    }
    return *this;
}

std::shared_ptr<CommandBuffer> CommandPool::allocateCommandBuffer()
{
    VOX_ASSERT(std::this_thread::get_id() == _creationThreadId, "");
    std::shared_ptr<CommandBuffer> outCommandBuffer = nullptr;
    if (_freedCommandBuffers.empty() == false)
    {
        if (_freedCommandBuffers.top()->getFenceToSignal().isCompleted())
        {
            outCommandBuffer = _freedCommandBuffers.top();
            _freedCommandBuffers.pop();
        }
    }

    if (outCommandBuffer == nullptr)
    {
        VkCommandBufferAllocateInfo allocInfo{
            .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO,
            .pNext = nullptr,
            .commandPool = _commandPool,
            .level = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
            .commandBufferCount = 1,
        };

        VkCommandBuffer vkCommandBuffer = VK_NULL_HANDLE;
        VK_ASSERT(vkAllocateCommandBuffers(_logicalDevice->get(), &allocInfo,
                                           &vkCommandBuffer));
        outCommandBuffer =
            std::make_shared<CommandBuffer>(_ownerQueue, this, vkCommandBuffer);
    }

    return outCommandBuffer;
}

void CommandPool::releaseCommandBuffer(
    std::shared_ptr<CommandBuffer>&& commandBuffer)
{
    VOX_ASSERT(std::this_thread::get_id() == _creationThreadId, "");
    _freedCommandBuffers.push(std::move(commandBuffer));
}
}  // namespace VoxFlow