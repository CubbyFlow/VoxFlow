// Author : snowapril

#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>

namespace VoxFlow
{
CommandBuffer::CommandBuffer(VkCommandBufferLevel cmdBufferLevel,
                             VkQueueFlagBits queueType,
                             bool instantBegin /* false */)
    : _queueType(queueType)
{
    // Do nothing
    (void)cmdBufferLevel;
    (void)instantBegin;
    (void)_queueType;
}
CommandBuffer::~CommandBuffer()
{
    // Do nothing
}

}  // namespace VoxFlow