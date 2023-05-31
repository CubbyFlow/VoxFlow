// Author : snowapril

#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandPool.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/FrameBuffer.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
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
                                       uint32_t backBufferIndex,
                                       const std::string& debugName)
{
    _swapChainIndexCached = swapChainIndex;
    _frameIndexCached = frameIndex;
    _backBufferIndexCached = backBufferIndex;
    _debugName = debugName;

    // Every resources and synchronization with this command buffer
    // will use below new allocated fence.
    _fenceToSignal = _commandQueue->allocateFenceToSignal();

    VOX_ASSERT(_hasBegun == false,
               "Duplicated beginning on the same CommandBuffer({})",
               _debugName);

    VkCommandBufferBeginInfo beginInfo = {
        .sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO,
        .pNext = nullptr,
        .flags = VK_COMMAND_BUFFER_LEVEL_PRIMARY,
        .pInheritanceInfo = nullptr,
    };
    vkBeginCommandBuffer(_vkCommandBuffer, &beginInfo);
    _hasBegun = true;

#if defined(VK_DEBUG_NAME_ENABLED)
    const std::string cmdDebugName =
        fmt::format("{}_SwapChainIndex({})_FrameIndex({})_BackBufferIndex({})",
                    _debugName, _frameContext._swapChainIndex,
                    _frameContext._frameIndex, _frameContext._backBufferIndex);
    DebugUtil::setObjectName(_logicalDevice, _vkCommandBuffer,
                             cmdDebugName.c_str());
#endif
}

void CommandBuffer::endCommandBuffer()
{
    vkEndCommandBuffer(_vkCommandBuffer);
    _hasBegun = false;
}

void CommandBuffer::beginRenderPass(const RenderTargetsInfo& rtInfo)
{
    LogicalDevice* logicalDevice = _commandQueue->getLogicalDevice();
    RenderPassCollector* renderPassCollector =
        logicalDevice->getRenderPassCollector();

    _boundRenderPass =
        renderPassCollector->getOrCreateRenderPass(rtInfo._layoutKey);

    auto frameBuffer =
        renderPassCollector->getOrCreateFrameBuffer(_boundRenderPass, rtInfo);

    std::vector<VkClearValue> clearValues;
    for (const ColorPassDescription& colorPass :
         rtInfo._layoutKey._colorAttachmentDescs)
    {
        clearValues.push_back(
            VkClearValue{ .color = VkClearColorValue{
                              .float32 = { colorPass._clearColorValues.x,
                                           colorPass._clearColorValues.y,
                                           colorPass._clearColorValues.z,
                                           colorPass._clearColorValues.w } } });
    }

    if (rtInfo._layoutKey._depthStencilAttachment.has_value())
    {
        const DepthStencilPassDescription& depthPass =
            rtInfo._layoutKey._depthStencilAttachment.value();
        clearValues.push_back(
            VkClearValue{ .depthStencil = VkClearDepthStencilValue{
                              .depth = depthPass._clearDepthValue,
                              .stencil = depthPass._clearStencilValue } });
    }

    VkRenderPassBeginInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO,
        .pNext = nullptr,
        .renderPass = _boundRenderPass->get(),
        .framebuffer = frameBuffer->get(),
        .renderArea = { .offset = { 0U, 0U },
                        .extent = { rtInfo._resolution.x,
                                    rtInfo._resolution.y } },
        .clearValueCount = static_cast<uint32_t>(clearValues.size()),
        .pClearValues = clearValues.data()
    };
    vkCmdBeginRenderPass(_vkCommandBuffer, &renderPassInfo,
                         VK_SUBPASS_CONTENTS_INLINE);

}

void CommandBuffer::endRenderPass()
{
    vkCmdEndRenderPass(_vkCommandBuffer);
    _boundRenderPass.reset();
}

void CommandBuffer::bindPipeline(const std::shared_ptr<BasePipeline>& pipeline)
{
    vkCmdBindPipeline(_vkCommandBuffer, pipeline->getBindPoint(),
                      pipeline->get());

    // TODO(snowapril) : below code will be moved.
    vkCmdDraw(_vkCommandBuffer, 4, 1, 0, 0);
}

void CommandBuffer::setViewport(const glm::uvec2& viewportSize)
{
    VkViewport viewport = { .x = 0,
                            .y = 0,
                            .width = static_cast<float>(viewportSize.x),
                            .height = static_cast<float>(viewportSize.y),
                            .minDepth = 0.0f,
                            .maxDepth = 1.0f };
    vkCmdSetViewport(_vkCommandBuffer, 0, 1, &viewport);

    VkRect2D scissor = { .offset = { .x = 0, .y = 0 },
                         .extent = { .width = viewportSize.x,
                                     .height = viewportSize.y } };
    vkCmdSetScissor(_vkCommandBuffer, 0, 1, &scissor);
}

void CommandBuffer::makeSwapChainFinalLayout(
    const std::shared_ptr<SwapChain>& swapChain)
{
    VkImageMemoryBarrier imageMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = 0,
        .dstAccessMask = 0,
        .oldLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = swapChain->getSwapChainImage(_backBufferIndexCached)->get(),
        .subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                              .baseMipLevel = 0,
                              .levelCount = 1,
                              .baseArrayLayer = 0,
                              .layerCount = 1 },
    };

    vkCmdPipelineBarrier(_vkCommandBuffer,
                         VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
                         VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
                         VK_DEPENDENCY_BY_REGION_BIT, 0, nullptr, 0, nullptr, 1,
                         &imageMemoryBarrier);
}

}  // namespace VoxFlow