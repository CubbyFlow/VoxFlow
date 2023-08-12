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
#include <VoxFlow/Core/Resources/BindableResourceView.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/StagingBuffer.hpp>
#include <VoxFlow/Core/Resources/ResourceTracker.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
CommandBuffer::CommandBuffer(LogicalDevice* logicalDevice,
                             VkCommandBuffer vkCommandBuffer)
    : _logicalDevice(logicalDevice), _vkCommandBuffer(vkCommandBuffer)
{
}
CommandBuffer::~CommandBuffer()
{
    // Do nothing
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

    // TODO(snowapril) :
    RenderTargetLayoutKey rtLayoutKey = {};

    _boundRenderPass = renderPassCollector->getOrCreateRenderPass(rtLayoutKey);

    RenderTargetsInfo rtInfo = {};
    rtInfo._vkRenderPass = _boundRenderPass->get();

    auto frameBuffer = renderPassCollector->getOrCreateFrameBuffer(rtInfo);

    std::vector<VkClearValue> clearValues;
    for (const ColorPassDescription& colorPass :
         rtLayoutKey._colorAttachmentDescs)
    {
        clearValues.push_back(
            VkClearValue{ .color = VkClearColorValue{
                              .float32 = { colorPass._clearColorValues.x,
                                           colorPass._clearColorValues.y,
                                           colorPass._clearColorValues.z,
                                           colorPass._clearColorValues.w } } });
    }

    if (rtLayoutKey._depthStencilAttachment.has_value())
    {
        const DepthStencilPassDescription& depthPass =
            rtLayoutKey._depthStencilAttachment.value();
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
    _boundRenderPass = nullptr;
}

void CommandBuffer::bindVertexBuffer(Buffer* vertexBuffer)
{
    // TODO(snowapril) : 
    VkBuffer vkVertexBuffer = vertexBuffer->get();
    vkCmdBindVertexBuffers(_vkCommandBuffer, 0, 1, &vkVertexBuffer, nullptr);
}

void CommandBuffer::bindIndexBuffer(Buffer* indexBuffer)
{
    // TODO(snowapril) : 
    VkBuffer vkIndexBuffer = indexBuffer->get();
    vkCmdBindIndexBuffer(_vkCommandBuffer, vkIndexBuffer, 0,
                         VK_INDEX_TYPE_UINT16);
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
    std::vector<ShaderVariable>&& shaderVariables)
{
    std::vector<ShaderVariable>& dstBindingResources =
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
    PipelineLayout* pipelineLayout = _boundPipeline->getPipelineLayout();
    for (uint32_t setIndex = 1; setIndex < MAX_NUM_SET_SLOTS; ++setIndex)
    {
        const SetSlotCategory setSlotCategory =
            static_cast<SetSlotCategory>(setIndex);

        std::vector<ShaderVariable>&
            bindGroup = _pendingResourceBindings[setIndex];

        DescriptorSetAllocator* setAllocator =
            pipelineLayout->getDescSetAllocator(setSlotCategory);
        const DescriptorSetLayoutDesc& setLayoutDesc =
            setAllocator->getDescriptorSetLayoutDesc();

        VkDescriptorSet pooledDescriptorSet =
            static_cast<PooledDescriptorSetAllocator*>(setAllocator)
                ->getOrCreatePooledDescriptorSet(_fenceToSignal);

        std::vector<VkWriteDescriptorSet> vkWrites;
        vkWrites.reserve(bindGroup.size());

        static VkDescriptorImageInfo sTmpImageInfo = {};
        static VkDescriptorBufferInfo sTmpBufferInfo = {};

        for (const ShaderVariable& resourceBinding : bindGroup)
        {
            const std::string_view& resourceBindingName = resourceBinding._variableName;
            BindableResourceView* bindingResourceView = resourceBinding._view;

            const VkDescriptorImageInfo* imageInfo = nullptr;
            const VkDescriptorBufferInfo* bufferInfo = nullptr;

            // TODO(snowapril) : figure out other way to find descriptor type
            // for given binding name
            // DescriptorSetLayoutDesc::ContainerType::const_iterator it =
            //    setLayoutDesc._bindingMap.find(
            //        std::string(resourceBindingName));
            //
            //VOX_ASSERT(it != setLayoutDesc._bindingMap.end(),
            //           "Unknown binding name ({})", resourceBindingName);

            uint32_t binding = 0;
            uint32_t arraySize = 0;

            VkDescriptorType vkDescriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;

            //std::visit(
            //    overloaded{
            //        [&binding, &arraySize, &vkDescriptorType](
            //            DescriptorSetLayoutDesc::CombinedImage setBinding) {
            //            binding = setBinding._binding;
            //            arraySize = setBinding._arraySize;
            //            vkDescriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            //        },
            //        [&binding, &arraySize, &vkDescriptorType](
            //            DescriptorSetLayoutDesc::UniformBuffer setBinding) {
            //            binding = setBinding._binding;
            //            arraySize = setBinding._arraySize;
            //            vkDescriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            //        },
            //        [&binding, &arraySize, &vkDescriptorType](
            //            DescriptorSetLayoutDesc::StorageBuffer setBinding) {
            //            binding = setBinding._binding;
            //            arraySize = setBinding._arraySize;
            //            vkDescriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            //        },
            //    },
            //    it->second);

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

void CommandBuffer::uploadBuffer(Buffer* dstBuffer, StagingBuffer* srcBuffer,
    const uint32_t dstOffset, const uint32_t srcOffset,
    const uint32_t size)
{
    const VkBufferCopy bufferCopy = { .srcOffset = srcOffset,
                                      .dstOffset = dstOffset,
                                      .size = size };

    VkBuffer srcVkBuffer = srcBuffer->get();
    VkBuffer dstVkBuffer = dstBuffer->get();

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

void CommandBuffer::makeResourceLayout(BindableResourceView* resourceView,
                                       const DescriptorInfo& descInfo)
{
    (void)resourceView;
    (void)descInfo;
}

}  // namespace VoxFlow