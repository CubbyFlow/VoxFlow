// Author : snowapril

#include <VoxFlow/Core/Graphics/Commands/ResourceBarrierManager.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Resources/StagingBuffer.hpp>

namespace VoxFlow
{

VkAccessFlags estimateAccessFlags(ResourceAccessMask accessMask)
{
    VkAccessFlags finalAccessFlags = VK_ACCESS_NONE;

    if (uint32_t(accessMask & ResourceAccessMask::TransferSource) > 0)
        finalAccessFlags |= VK_ACCESS_TRANSFER_READ_BIT;
    if (uint32_t(accessMask & ResourceAccessMask::TransferDest) > 0)
        finalAccessFlags |= VK_ACCESS_TRANSFER_WRITE_BIT;
    if (uint32_t(accessMask & ResourceAccessMask::VertexBuffer) > 0)
        finalAccessFlags |= VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;
    if (uint32_t(accessMask & ResourceAccessMask::IndexBuffer) > 0)
        finalAccessFlags |= VK_ACCESS_INDEX_READ_BIT;
    if (uint32_t(accessMask & ResourceAccessMask::ColorAttachment) > 0)
        finalAccessFlags |= VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                            VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    if ((uint32_t(accessMask & ResourceAccessMask::DepthAttachment) > 0) ||
        (uint32_t(accessMask & ResourceAccessMask::StencilAttachment) > 0))
        finalAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                            VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
    if ((uint32_t(accessMask & ResourceAccessMask::DepthReadOnly) > 0) ||
        (uint32_t(accessMask & ResourceAccessMask::StencilReadOnly) > 0))
        finalAccessFlags |= VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
    if (uint32_t(accessMask & ResourceAccessMask::ShaderReadOnly) > 0)
        finalAccessFlags |= VK_ACCESS_SHADER_READ_BIT;
    if (uint32_t(accessMask & ResourceAccessMask::General) > 0)
        finalAccessFlags |=
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
    if (uint32_t(accessMask & ResourceAccessMask::StorageBuffer) > 0)
        finalAccessFlags |=
            VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;
    if (uint32_t(accessMask & ResourceAccessMask::UniformBuffer) > 0)
        finalAccessFlags |= VK_ACCESS_SHADER_READ_BIT;

    return finalAccessFlags;
}

VkImageLayout estimateImageLayout(ResourceAccessMask accessMask)
{
    VkImageLayout imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;

    if (uint32_t(accessMask & ResourceAccessMask::General) > 0)
        imageLayout = VK_IMAGE_LAYOUT_GENERAL;
    else if (uint32_t(accessMask & ResourceAccessMask::ColorAttachment) > 0)
        imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    else if ((uint32_t(accessMask & ResourceAccessMask::DepthAttachment) > 0) ||
             (uint32_t(accessMask & ResourceAccessMask::StencilAttachment) > 0))
        imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
    else if ((uint32_t(accessMask & ResourceAccessMask::DepthReadOnly) > 0) ||
             (uint32_t(accessMask & ResourceAccessMask::StencilReadOnly) > 0))
        imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
    else if (uint32_t(accessMask & ResourceAccessMask::TransferSource) > 0)
        imageLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    else if (uint32_t(accessMask & ResourceAccessMask::TransferDest) > 0)
        imageLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    else if (uint32_t(accessMask & ResourceAccessMask::ShaderReadOnly) > 0)
        imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

    return imageLayout;
}

void ResourceBarrierManager::addGlobalMemoryBarrier(
    ResourceAccessMask prevAccessMasks, ResourceAccessMask nextAccessMasks)
{
    _globalMemoryBarrier._srcAccessFlags = estimateAccessFlags(prevAccessMasks);
    _globalMemoryBarrier._dstAccessFlags = estimateAccessFlags(nextAccessMasks);
}

void ResourceBarrierManager::addTextureMemoryBarrier(
    TextureView* textureView, ResourceAccessMask accessMask,
    VkShaderStageFlags nextStageFlags)
{
    Texture* texture = static_cast<Texture*>(textureView->getOwnerResource());
    // TODO(snowapril) : get dstQueueFamilyIndex from command buffer

    const TextureViewInfo& textureViewInfo = textureView->getViewInfo();
    const VkImageLayout nextImageLayout = estimateImageLayout(accessMask);

    _memoryBarrierGroup._srcStageFlags |=
        textureView->getLastusedShaderStageFlags();
    _memoryBarrierGroup._dstStageFlags |= nextStageFlags;
    _memoryBarrierGroup._imageBarriers.push_back(VkImageMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = estimateAccessFlags(textureView->getLastAccessMask()),
        .dstAccessMask = estimateAccessFlags(accessMask),
        .oldLayout = estimateImageLayout(textureView->getLastAccessMask()),
        .newLayout = nextImageLayout,
        .srcQueueFamilyIndex = texture->getCurrentQueueFamilyIndex(),
        .dstQueueFamilyIndex = texture->getCurrentQueueFamilyIndex(),
        .image = texture->get(),
        .subresourceRange =
            VkImageSubresourceRange{
                .aspectMask = textureViewInfo._aspectFlags,
                .baseMipLevel = textureViewInfo._baseMipLevel,
                .levelCount = textureViewInfo._levelCount,
                .baseArrayLayer = textureViewInfo._baseMipLevel,
                .layerCount = textureViewInfo._layerCount },
    });

    textureView->setLastAccessMask(accessMask);
    textureView->setCurrentVkImageLayout(nextImageLayout);
}

void ResourceBarrierManager::addBufferMemoryBarrier(
    BufferView* bufferView, ResourceAccessMask accessMask,
    VkShaderStageFlags nextStageFlags)
{
    Buffer* buffer = static_cast<Buffer*>(bufferView->getOwnerResource());
    // TODO(snowapril) : get dstQueueFamilyIndex from command buffer

    const BufferViewInfo& bufferViewInfo = bufferView->getViewInfo();

    _memoryBarrierGroup._srcStageFlags |=
        bufferView->getLastusedShaderStageFlags();
    _memoryBarrierGroup._dstStageFlags |= nextStageFlags;
    _memoryBarrierGroup._bufferBarriers.push_back(VkBufferMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask = estimateAccessFlags(bufferView->getLastAccessMask()),
        .dstAccessMask = estimateAccessFlags(accessMask),
        .srcQueueFamilyIndex = buffer->getCurrentQueueFamilyIndex(),
        .dstQueueFamilyIndex = buffer->getCurrentQueueFamilyIndex(),
        .buffer = buffer->get(),
        .offset = bufferViewInfo._offset,
        .size = bufferViewInfo._range,
    });

    bufferView->setLastAccessMask(accessMask);
}

void ResourceBarrierManager::addStagingBufferMemoryBarrier(
    StagingBufferView* stagingBufferView, ResourceAccessMask accessMask,
    VkShaderStageFlags nextStageFlags)
{
    StagingBuffer* stagingBuffer =
        static_cast<StagingBuffer*>(stagingBufferView->getOwnerResource());
    // TODO(snowapril) : get dstQueueFamilyIndex from command buffer

    const BufferViewInfo& stagingBufferViewInfo =
        stagingBufferView->getViewInfo();

    _memoryBarrierGroup._srcStageFlags |=
        stagingBufferView->getLastusedShaderStageFlags();
    _memoryBarrierGroup._dstStageFlags |= nextStageFlags;
    _memoryBarrierGroup._bufferBarriers.push_back(VkBufferMemoryBarrier{
        .sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER,
        .pNext = nullptr,
        .srcAccessMask =
            estimateAccessFlags(stagingBufferView->getLastAccessMask()),
        .dstAccessMask = estimateAccessFlags(accessMask),
        .srcQueueFamilyIndex = stagingBuffer->getCurrentQueueFamilyIndex(),
        .dstQueueFamilyIndex = stagingBuffer->getCurrentQueueFamilyIndex(),
        .buffer = stagingBuffer->get(),
        .offset = stagingBufferViewInfo._offset,
        .size = stagingBufferViewInfo._range,
    });

    stagingBufferView->setLastAccessMask(accessMask);
}

void ResourceBarrierManager::addExecutionBarrier(
    VkShaderStageFlags prevStageFlags, VkShaderStageFlags nextStageFlags)
{
    _executionBarrier._srcStageFlags = prevStageFlags;
    _executionBarrier._dstStageFlags = nextStageFlags;
}

void ResourceBarrierManager::commitPendingBarriers(const bool inRenderPassScope)
{
    const VkDependencyFlags dependencyFlag =
        inRenderPassScope ? VK_DEPENDENCY_BY_REGION_BIT : 0;

    const VkCommandBuffer vkCommandBuffer = _commandBuffer->get();
    if (_globalMemoryBarrier.isValid() || _executionBarrier.isValid())
    {
        const VkMemoryBarrier memoryBarrier = {
            .sType = VK_STRUCTURE_TYPE_MEMORY_BARRIER,
            .pNext = nullptr,
            .srcAccessMask = _globalMemoryBarrier._srcAccessFlags,
            .dstAccessMask = _globalMemoryBarrier._dstAccessFlags
        };
        vkCmdPipelineBarrier(vkCommandBuffer, _executionBarrier._srcStageFlags,
                             _executionBarrier._dstStageFlags, dependencyFlag,
                             1, &memoryBarrier, 0, nullptr, 0, nullptr);

        _globalMemoryBarrier.reset();
        _executionBarrier.reset();
    }

    if (_memoryBarrierGroup.isValid())
    {
        vkCmdPipelineBarrier(
            vkCommandBuffer, _memoryBarrierGroup._srcStageFlags,
            _memoryBarrierGroup._dstStageFlags, dependencyFlag, 0, nullptr,
            static_cast<uint32_t>(_memoryBarrierGroup._bufferBarriers.size()),
            _memoryBarrierGroup._bufferBarriers.data(),
            static_cast<uint32_t>(_memoryBarrierGroup._imageBarriers.size()),
            _memoryBarrierGroup._imageBarriers.data());

        _memoryBarrierGroup.reset();
    }
}

}  // namespace VoxFlow