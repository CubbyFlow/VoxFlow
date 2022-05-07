// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_BUFFER_HPP
#define VOXEL_FLOW_COMMAND_BUFFER_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>

namespace VoxFlow
{
class CommandBuffer : NonCopyable
{
 public:
    explicit CommandBuffer(VkCommandBufferLevel cmdBufferLevel,
                           VkQueueFlagBits queueType,
                           bool instantBegin = false);
    ~CommandBuffer();

    [[nodiscard]] VkCommandBuffer get() const noexcept
    {
        return _commandBuffer;
    }

 private:
    VkCommandBuffer _commandBuffer{ VK_NULL_HANDLE };
    VkQueueFlagBits _queueType;
};
}  // namespace VoxFlow

#endif