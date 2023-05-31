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

    vkCmdPipelineBarrier(
        _vkCommandBuffer, VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
        VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT, VK_DEPENDENCY_BY_REGION_BIT, 0,
        nullptr, 0, nullptr, 1, &imageMemoryBarrier);
}

void CommandBuffer::bindResourceGroup(
    SetSlotCategory setSlotCategory,
    std::vector<std::pair<std::string_view, BindableResourceView*>>&& bindGroup)
{
    std::vector<std::pair<std::string_view, BindableResourceView*>>&
        dstBindingResources =
            _pendingResourceBindings[static_cast<uint32_t>(setSlotCategory)];

    if (dstBindingResources.empty())
    {
        dstBindingResources = std::move(bindGroup);
    }
    else
    {
        dstBindingResources.reserve(dstBindingResources.size() +
                                    bindGroup.size());
        std::move(std::make_move_iterator(bindGroup.begin()),
                  std::make_move_iterator(bindGroup.end()),
                  std::back_inserter(dstBindingResources));
    }
}

void CommandBuffer::commitPendingResourceBindings()
{
    for (uint32_t setIndex = 0; setIndex < MAX_NUM_SET_SLOTS; ++setIndex)
    {
        const SetSlotCategory setSlotCategory =
            static_cast<SetSlotCategory>(setIndex);
        std::vector<std::pair<std::string_view, BindableResourceView*>>&
            bindGroup = _pendingResourceBindings[setIndex];
        PipelineLayout* pipelineLayout = _boundPipeline->getPipelineLayout();
        DescriptorSetAllocator* setAllocator =
            pipelineLayout->getDescSetAllocator(setSlotCategory);
        const DescriptorSetLayoutDesc& setLayoutDesc =
            setAllocator->getDescriptorSetLayoutDesc();

        VkDescriptorSet pooledDescriptorSet =
            setAllocator->getOrCreatePooledDescriptorSet(_fenceToSignal);

        std::vector<VkWriteDescriptorSet> vkWrites;
        vkWrites.reserve(bindGroup.size());

        static VkDescriptorImageInfo sTmpImageInfo = {};
        static VkDescriptorBufferInfo sTmpBufferInfo = {};

        for (const std::pair<std::string_view, BindableResourceView*>&
                 resourceBinding : bindGroup)
        {
            const std::string_view& resourceBindingName = resourceBinding.first;
            BindableResourceView* bindingResourceView = resourceBinding.second;

            const VkDescriptorImageInfo* imageInfo = nullptr;
            const VkDescriptorBufferInfo* bufferInfo = nullptr;

            DescriptorSetLayoutDesc::ContainerType::const_iterator it =
                setLayoutDesc._bindingMap.find(
                    std::string(resourceBindingName));

            VOX_ASSERT(it != setLayoutDesc._bindingMap.end(),
                       "Unknown binding name ({})", resourceBindingName);

            uint32_t binding = 0;
            uint32_t arraySize = 0;

            VkDescriptorType vkDescriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

            std::visit(
                overloaded{
                    [this, &binding, &arraySize, &vkDescriptorType](
                        DescriptorSetLayoutDesc::SampledImage setBinding) {
                        binding = setBinding._binding;
                        arraySize = setBinding._arraySize;
                        vkDescriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                    },
                    [this, &binding, &arraySize, &vkDescriptorType](
                        DescriptorSetLayoutDesc::UniformBuffer setBinding) {
                        binding = setBinding._binding;
                        arraySize = setBinding._arraySize;
                        vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    },
                    [this, &binding, &arraySize, &vkDescriptorType](
                        DescriptorSetLayoutDesc::StorageBuffer setBinding) {
                        binding = setBinding._binding;
                        arraySize = setBinding._arraySize;
                        vkDescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    },
                },
                it->second);

            VOX_ASSERT(vkDescriptorType != VK_DESCRIPTOR_TYPE_MAX_ENUM,
                       "Unknown descriptor type {}", vkDescriptorType);

            switch (bindingResourceView->getResourceViewType())
            {
                case ResourceViewType::BufferView:
                    sTmpBufferInfo =
                        static_cast<BufferView*>(bindingResourceView)
                            ->getDescriptorBufferInfo();
                    bufferInfo = &sTmpBufferInfo;
                    break;
                case ResourceViewType::ImageView:
                    sTmpImageInfo =
                        static_cast<TextureView*>(bindingResourceView)
                            ->getDescriptorImageInfo();

                    // TODO(snowapril) : set image layout
                    // sTmpImageInfo.imageLayout = ;

                    imageInfo = &sTmpImageInfo;
                    break;
            }

            // TODO(snowapril) : get below information from descriptor set
            // layout desc
            VkWriteDescriptorSet vkWrite = {
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = pooledDescriptorSet,
                .dstBinding = static_cast<uint32_t>(setSlotCategory),
                .dstArrayElement = binding,
                .descriptorCount = arraySize,
                .descriptorType = vkDescriptorType,
                .pImageInfo = imageInfo,
                .pBufferInfo = bufferInfo,
                .pTexelBufferView = nullptr
            };

            vkWrites.push_back(vkWrite);
        }

        vkUpdateDescriptorSets(_logicalDevice->get(),
                               static_cast<uint32_t>(vkWrites.size()),
                               vkWrites.data(), 0, nullptr);

        vkCmdBindDescriptorSets(
            _vkCommandBuffer, _boundPipeline->getBindPoint(),
            pipelineLayout->get(), static_cast<uint32_t>(setSlotCategory), 1,
            &pooledDescriptorSet, 0, nullptr);

        bindGroup.clear();
    }
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                uint32_t firstIndex, int32_t vertexOffset,
                                uint32_t firstInstance)
{
    commitPendingResourceBindings();

    vkCmdDrawIndexed(_vkCommandBuffer, indexCount, instanceCount, firstIndex,
                     vertexOffset, firstInstance);
}

void CommandBuffer::makeResourceLayout(
    BindableResourceView* resourceView,
    const DescriptorSetLayoutDesc::DescriptorType& descriptorDesc)
{
    (void)resourceView;
    (void)descriptorDesc;
}

}  // namespace VoxFlow