// Author : snowapril

#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandPool.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/FrameBuffer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
CommandBuffer::CommandBuffer(Queue* commandQueue, CommandPool* commandPool,
                             VkCommandBuffer vkCommandBuffer)
    : _commandQueue(commandQueue),
      // _ownerCommandPool(commandPool),
      _vkCommandBuffer(vkCommandBuffer)
{
    (void)commandPool;
}
CommandBuffer::~CommandBuffer()
{
    // Do nothing
}

void CommandBuffer::beginCommandBuffer(uint32_t swapChainIndex,
                                       uint32_t frameIndex,
                                       const std::string& debugName)
{
    _swapChainIndexCached = swapChainIndex;
    _frameIndexCached = frameIndex;
    _debugName = debugName;

    _fenceToSignal = FenceObject(
        _commandQueue, _commandQueue->getLastExecutedFenceValue() + 1);

    VOX_ASSERT(_hasBegun == false,
               "Duplicated beginning on the same CommandBuffer(%s)",
               _debugName);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .pInheritanceInfo = nullptr,
    };
    vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo);
    _hasBegun = true;
}

void CommandBuffer::endCommandBuffer()
{
    vkEndCommandBuffer(_vkCommandBuffer);
    _hasBegun = false;
}
}  // namespace VoxFlow