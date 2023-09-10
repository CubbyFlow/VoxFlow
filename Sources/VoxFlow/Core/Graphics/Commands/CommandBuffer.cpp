// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandPool.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/FrameBuffer.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp>
#include <VoxFlow/Core/Resources/ResourceView.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/StagingBuffer.hpp>
#include <VoxFlow/Core/Resources/ResourceTracker.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Resources/Sampler.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
CommandBuffer::CommandBuffer(LogicalDevice* logicalDevice,
                             VkCommandBuffer vkCommandBuffer)
    : _logicalDevice(logicalDevice),
      _vkCommandBuffer(vkCommandBuffer),
      _resourceBarrierManager(this)
{
    // TODO(snowapril) : temporal sampler
    _sampler = new Sampler("TempSampler", logicalDevice);
    _sampler->initialize();
}

CommandBuffer::~CommandBuffer()
{
    // Do nothing
    if (_sampler)
    {
        delete _sampler;
    }
}

void CommandBuffer::beginCommandBuffer(const FenceObject& fenceToSignal,
                                       const std::string& debugName)
{
    _debugName = debugName;

    // Every resources and synchronization with this command buffer
    // will use below new allocated fence.
    _fenceToSignal = fenceToSignal;

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
    DebugUtil::setObjectName(_logicalDevice, _vkCommandBuffer,
                             _debugName.c_str());
#endif
}

void CommandBuffer::endCommandBuffer()
{
    vkEndCommandBuffer(_vkCommandBuffer);
    _hasBegun = false;
}

void CommandBuffer::beginRenderPass(const AttachmentGroup& attachmentGroup,
                                    const RenderPassParams& passParams)
{
    RenderPassCollector* renderPassCollector =
        _logicalDevice->getRenderPassCollector();

    const uint32_t numColorAttachments =
        attachmentGroup.getNumColorAttachments();
    const bool hasDepthStencil = attachmentGroup.hasDepthStencil();

    RenderTargetLayoutKey rtLayoutKey = {};
    RenderTargetsInfo rtInfo = {};

    for (uint32_t i = 0; i < numColorAttachments; ++i)
    {
        TextureView* textureView = attachmentGroup.getColor(i).getView();
        const TextureViewInfo& viewInfo = textureView->getViewInfo();
        rtLayoutKey._colorFormats.emplace_back(viewInfo._format);

        rtInfo._colorRenderTarget.emplace_back(textureView);
    }

    if (hasDepthStencil)
    {
        TextureView* textureView = attachmentGroup.getDepthStencil().getView();
        const TextureViewInfo& viewInfo = textureView->getViewInfo();
        rtLayoutKey._depthStencilFormat = viewInfo._format;

        rtInfo._depthStencilImage = textureView;
    }

    rtLayoutKey._renderPassFlags = passParams._attachmentFlags;

    _boundRenderPass = renderPassCollector->getOrCreateRenderPass(rtLayoutKey);
    rtInfo._vkRenderPass = _boundRenderPass->get();
    rtInfo._resolution = passParams._viewportSize;

    // rtInfo._resolution = 0;
    // rtInfo._layers = 0;
    // rtInfo._numSamples = 0;

    auto frameBuffer = renderPassCollector->getOrCreateFrameBuffer(rtInfo);

    std::vector<VkClearValue> clearValues;
    for (uint32_t i = 0; i < numColorAttachments; ++i)
    {
        const glm::vec4& clearColorValue = passParams._clearColors[i];
        clearValues.push_back(
            VkClearValue{ .color = VkClearColorValue{
                              .float32 = { clearColorValue.x,
                                           clearColorValue.y,
                                           clearColorValue.z,
                                           clearColorValue.w } } });
    }

    if (hasDepthStencil)
    {
        clearValues.push_back(
            VkClearValue{ .depthStencil = VkClearDepthStencilValue{
                              .depth = passParams._clearDepth,
                              .stencil = passParams._clearStencil } });
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

    _isInRenderPassScope = true;
}

void CommandBuffer::endRenderPass()
{
    vkCmdEndRenderPass(_vkCommandBuffer);
    _boundRenderPass = nullptr;
    _isInRenderPassScope = false;
}

void CommandBuffer::bindVertexBuffer(Buffer* vertexBuffer)
{
    // TODO(snowapril) : must implement details
    VkBuffer vkVertexBuffer = vertexBuffer->get();
    const VkDeviceSize offsets[] = { 0 };
    vkCmdBindVertexBuffers(_vkCommandBuffer, 0, 1, &vkVertexBuffer, offsets);
}

void CommandBuffer::bindIndexBuffer(Buffer* indexBuffer)
{
    // TODO(snowapril) : must implement details
    VkBuffer vkIndexBuffer = indexBuffer->get();
    vkCmdBindIndexBuffer(_vkCommandBuffer, vkIndexBuffer, 0,
                         VK_INDEX_TYPE_UINT32);
}

void CommandBuffer::bindPipeline(BasePipeline* pipeline)
{
    _boundPipeline = pipeline;

    if (_boundPipeline->validatePipeline() == false)
    {
        VkPipelineBindPoint bindPoint = _boundPipeline->getBindPoint();
        switch (bindPoint)
        {
            case VK_PIPELINE_BIND_POINT_GRAPHICS:
                static_cast<GraphicsPipeline*>(_boundPipeline)->initialize(_boundRenderPass);
                break;

            case VK_PIPELINE_BIND_POINT_COMPUTE:
                static_cast<ComputePipeline*>(_boundPipeline)->initialize();
                break;

            default:
                VOX_ASSERT(false, "Failed to find valid bind point");
        }
    }

    vkCmdBindPipeline(_vkCommandBuffer, _boundPipeline->getBindPoint(),
                      _boundPipeline->get());
}

void CommandBuffer::unbindPipeline()
{
    _boundPipeline = nullptr;
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

void CommandBuffer::makeSwapChainFinalLayout(SwapChain* swapChain, const uint32_t backBufferIndex)
{
    VkImageMemoryBarrier imageMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = 0,
        .dstAccessMask = 0,
        .oldLayout = VK_IMAGE_LAYOUT_UNDEFINED,
        .newLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
        .srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
        .image = swapChain->getSwapChainImage(backBufferIndex)->get(),
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
    std::vector<ShaderVariableBinding>&& shaderVariables)
{
    std::vector<ShaderVariableBinding>& dstBindingResources =
        _pendingResourceBindings[static_cast<uint32_t>(setSlotCategory)];

    if (dstBindingResources.empty())
    {
        dstBindingResources = std::move(shaderVariables);
    }
    else
    {
        dstBindingResources.reserve(dstBindingResources.size() +
                                    shaderVariables.size());
        std::move(std::make_move_iterator(shaderVariables.begin()),
                  std::make_move_iterator(shaderVariables.end()),
                  std::back_inserter(dstBindingResources));
    }
}

void CommandBuffer::commitPendingResourceBindings()
{
    // TODO(snowapril) : split update descriptor sets according to set frequency
    const PipelineLayout* pipelineLayout = _boundPipeline->getPipelineLayout();
    
    const PipelineLayout::ShaderVariableMap& shaderVariableMap =
        pipelineLayout->getShaderVariableMap();
    const PipelineLayoutDescriptor& pipelineLayoutDesc =
        pipelineLayout->getPipelineLayoutDescriptor();

    for (uint32_t setIndex = 1; setIndex < MAX_NUM_SET_SLOTS; ++setIndex)
    {
        const SetSlotCategory setSlotCategory =
            static_cast<SetSlotCategory>(setIndex);

        std::vector<ShaderVariableBinding>& bindGroup =
            _pendingResourceBindings[setIndex];

        DescriptorSetAllocator* setAllocator =
            pipelineLayout->getDescSetAllocator(setSlotCategory);
        const DescriptorSetLayoutDesc& setLayoutDesc =
            setAllocator->getDescriptorSetLayoutDesc();
        const size_t numDescriptors = setLayoutDesc._descriptorInfos.size();

        if (numDescriptors == 0ULL)
        {
            continue;
        }

        VkDescriptorSet pooledDescriptorSet =
            static_cast<PooledDescriptorSetAllocator*>(setAllocator)
                ->getOrCreatePooledDescriptorSet(_fenceToSignal);

        std::vector<VkWriteDescriptorSet> vkWrites;
        vkWrites.reserve(bindGroup.size());

        static thread_local std::vector<VkDescriptorImageInfo> sTmpImageInfos;
        static thread_local std::vector<VkDescriptorBufferInfo> sTmpBufferInfos;
        sTmpImageInfos.clear();
        sTmpImageInfos.resize(numDescriptors);
        sTmpBufferInfos.clear();
        sTmpBufferInfos.resize(numDescriptors);
        size_t currentDescriptorInfoIndex = 0;

        for (const ShaderVariableBinding& resourceBinding : bindGroup)
        {
            const std::string& resourceBindingName =
                resourceBinding._variableName;

            auto shaderVariableIter =
                shaderVariableMap.find(resourceBindingName);
            if (shaderVariableIter == shaderVariableMap.end())
            {
                VOX_ASSERT(false,
                           "Given shader variable name ({}) does not exist in "
                           "the current pipeline.",
                           resourceBindingName);
                continue;
            }

            const DescriptorInfo& descriptorInfo = shaderVariableIter->second;

            ResourceView* bindingResourceView = resourceBinding._view;

            addMemoryBarrier(bindingResourceView, resourceBinding._usage,
                             pipelineLayoutDesc._sets[setIndex]._stageFlags);

            const VkDescriptorImageInfo* imageInfo = nullptr;
            const VkDescriptorBufferInfo* bufferInfo = nullptr;

            uint32_t binding = descriptorInfo._binding;
            uint32_t arraySize = descriptorInfo._arraySize;
            VkDescriptorType vkDescriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
            switch (descriptorInfo._descriptorCategory)
            {
                case DescriptorCategory::CombinedImage:
                    vkDescriptorType =
                        VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    break;
                case DescriptorCategory::UniformBuffer:
                    vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                    break;
                case DescriptorCategory::StorageBuffer:
                    vkDescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                    break;
                default:
                    VOX_ASSERT(false,
                               "Unknown descriptor category must not be exist");
                    break;
            }

            switch (bindingResourceView->getResourceViewType())
            {
                case ResourceViewType::BufferView:
                    sTmpBufferInfos[currentDescriptorInfoIndex] =
                        static_cast<BufferView*>(bindingResourceView)
                            ->getDescriptorBufferInfo();
                    bufferInfo = &sTmpBufferInfos[currentDescriptorInfoIndex++];
                    break;
                case ResourceViewType::ImageView:
                    sTmpImageInfos[currentDescriptorInfoIndex] =
                        static_cast<TextureView*>(bindingResourceView)
                            ->getDescriptorImageInfo();

                    sTmpImageInfos[currentDescriptorInfoIndex].imageLayout =
                        static_cast<TextureView*>(bindingResourceView)
                            ->getCurrentVkImageLayout();

                    if (descriptorInfo._descriptorCategory ==
                        DescriptorCategory::CombinedImage)
                    {
                        sTmpImageInfos[currentDescriptorInfoIndex].sampler =
                            _sampler->get();
                    }

                    imageInfo = &sTmpImageInfos[currentDescriptorInfoIndex++];
                    break;
                case ResourceViewType::StagingBufferView:
                default:
                    VOX_ASSERT(false,
                               "Unhandled resource view type");
                    break;
            }

            vkWrites.push_back(VkWriteDescriptorSet{
                .sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
                .pNext = nullptr,
                .dstSet = pooledDescriptorSet,
                .dstBinding = static_cast<uint32_t>(binding),
                .dstArrayElement = 0,
                .descriptorCount = arraySize,
                .descriptorType = vkDescriptorType,
                .pImageInfo = imageInfo,
                .pBufferInfo = bufferInfo,
                .pTexelBufferView = nullptr });
        }

        // Note(snowapril) : As vulkan validation layer require that resource
        // views have already desired layout before vkUpdateDescriptorSets(not
        // before issuing draw/dispatch), commit barrier first.
        _resourceBarrierManager.commitPendingBarriers(_isInRenderPassScope);

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

void CommandBuffer::uploadBuffer(Buffer* dstBuffer, StagingBuffer* srcBuffer,
    const uint32_t dstOffset, const uint32_t srcOffset,
    const uint32_t size)
{
    const VkBufferCopy bufferCopy = { .srcOffset = srcOffset,
                                      .dstOffset = dstOffset,
                                      .size = size };

    VkBuffer srcVkBuffer = srcBuffer->get();
    VkBuffer dstVkBuffer = dstBuffer->get();

    addMemoryBarrier(dstBuffer->getDefaultView(),
                     ResourceAccessMask::TransferDest,
                     VK_PIPELINE_STAGE_TRANSFER_BIT);
    addMemoryBarrier(srcBuffer->getDefaultView(),
                     ResourceAccessMask::TransferSource,
                     VK_PIPELINE_STAGE_TRANSFER_BIT);
    _resourceBarrierManager.commitPendingBarriers(_isInRenderPassScope);

    vkCmdCopyBuffer(_vkCommandBuffer, srcVkBuffer, dstVkBuffer, 1, &bufferCopy);
}

void CommandBuffer::uploadTexture(Texture* dstTexture, StagingBuffer* srcBuffer,
    const uint32_t dstOffset, const uint32_t srcOffset,
    const uint32_t size)
{
    (void)dstTexture;
    (void)srcBuffer;
    (void)dstOffset;
    (void)srcOffset;
    (void)size;
}

void CommandBuffer::draw(uint32_t vertexCount, uint32_t instanceCount,
                         uint32_t firstVertex, uint32_t firstInstance)
{
    commitPendingResourceBindings();

    vkCmdDraw(_vkCommandBuffer, vertexCount, instanceCount, firstVertex,
              firstInstance);
}

void CommandBuffer::drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                                uint32_t firstIndex, int32_t vertexOffset,
                                uint32_t firstInstance)
{
    commitPendingResourceBindings();

    vkCmdDrawIndexed(_vkCommandBuffer, indexCount, instanceCount, firstIndex,
                     vertexOffset, firstInstance);
}

void CommandBuffer::addGlobalMemoryBarrier(ResourceAccessMask prevAccessMasks,
                                           ResourceAccessMask nextAccessMasks)
{
    _resourceBarrierManager.addGlobalMemoryBarrier(prevAccessMasks,
                                                   nextAccessMasks);
}

void CommandBuffer::addMemoryBarrier(ResourceView* view,
                                     ResourceAccessMask accessMask,
                                     VkPipelineStageFlags nextStageFlags)
{
    const ResourceViewType viewType = view->getResourceViewType();
    switch (viewType)
    {
        case ResourceViewType::BufferView:
            _resourceBarrierManager.addBufferMemoryBarrier(
                static_cast<BufferView*>(view), accessMask, nextStageFlags);
            break;
        case ResourceViewType::ImageView:
            _resourceBarrierManager.addTextureMemoryBarrier(
                static_cast<TextureView*>(view), accessMask, nextStageFlags);
            break;
        case ResourceViewType::StagingBufferView:
            _resourceBarrierManager.addStagingBufferMemoryBarrier(
                static_cast<StagingBufferView*>(view), accessMask,
                nextStageFlags);
            break;
    }
}

void CommandBuffer::addExecutionBarrier(VkPipelineStageFlags prevStageFlags,
                                        VkPipelineStageFlags nextStageFlags)
{
    _resourceBarrierManager.addExecutionBarrier(prevStageFlags, nextStageFlags);
}

}  // namespace VoxFlow