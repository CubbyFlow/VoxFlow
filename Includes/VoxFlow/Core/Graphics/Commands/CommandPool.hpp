// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_POOL_HPP
#define VOXEL_FLOW_COMMAND_POOL_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <queue>
#include <string>
#include <thread>

namespace VoxFlow
{
class LogicalDevice;
class Queue;
class CommandBuffer;

class CommandPool : private NonCopyable
{
 public:
    explicit CommandPool(LogicalDevice* logicalDevice, Queue* ownerQueue);
    ~CommandPool();
    CommandPool(CommandPool&& other) noexcept;
    CommandPool& operator=(CommandPool&& other) noexcept;

 public:
    // Create new command buffer or get a freed one
    std::shared_ptr<CommandBuffer> getOrCreateCommandBuffer();

    // Release command buffer to the freed pool
    void releaseCommandBuffer(std::shared_ptr<CommandBuffer>&& commandBuffer);

 private:
    struct CommandBufferComparator
    {
        bool operator()(const std::shared_ptr<CommandBuffer>& lhs, const std::shared_ptr<CommandBuffer>& rhs)
        {
            return lhs->getFenceToSignal().getFenceValue() > rhs->getFenceToSignal().getFenceValue();
        }
    };

    std::thread::id _creationThreadId;
    LogicalDevice* _logicalDevice = nullptr;
    Queue* _ownerQueue = nullptr;
    VkCommandPool _commandPool = VK_NULL_HANDLE;
    std::priority_queue<std::shared_ptr<CommandBuffer>, std::vector<std::shared_ptr<CommandBuffer>>, CommandBufferComparator> _freedCommandBuffers;
};
}  // namespace VoxFlow

#endif