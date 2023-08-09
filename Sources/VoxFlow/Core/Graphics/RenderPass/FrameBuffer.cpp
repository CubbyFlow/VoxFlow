// Author : snowapril

#include <VoxFlow/Core/Graphics/RenderPass/FrameBuffer.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
FrameBuffer::FrameBuffer(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
}

FrameBuffer::~FrameBuffer()
{
    release();
}

FrameBuffer::FrameBuffer(FrameBuffer&& other) noexcept
{
    operator=(std::move(other));
}

FrameBuffer& FrameBuffer::operator=(FrameBuffer&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = other._logicalDevice;
        _renderTargetsInfo = other._renderTargetsInfo;
        _vkFrameBuffer = other._vkFrameBuffer;
        other._vkFrameBuffer = VK_NULL_HANDLE;
    }
    return *this;
}

bool FrameBuffer::initialize(RenderPass* renderPass,
                             const RenderTargetsInfo& rtInfo)
{
    _renderTargetsInfo = rtInfo;

    std::vector<VkImageView> attachments;
    attachments.reserve(
        _renderTargetsInfo._colorRenderTarget.size() +
        (_renderTargetsInfo._depthStencilImage.has_value() ? 1U : 0U));
    
    for (const auto& colorRT : rtInfo._colorRenderTarget)
    {
        attachments.push_back(colorRT->get());
    }
    if (rtInfo._depthStencilImage.has_value())
    {
        attachments.push_back(rtInfo._depthStencilImage.value()->get());
    }

    VkFramebufferCreateInfo frameBufferInfo{
        .sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .renderPass = renderPass->get(),
        .attachmentCount = static_cast<uint32_t>(attachments.size()),
        .pAttachments = attachments.data(),
        .width = _renderTargetsInfo._resolution.x,
        .height = _renderTargetsInfo._resolution.y,
        .layers = 1,
    };

    if (attachments.empty())
    {
        frameBufferInfo.flags = VK_FRAMEBUFFER_CREATE_IMAGELESS_BIT;
    }

    VK_ASSERT(vkCreateFramebuffer(_logicalDevice->get(), &frameBufferInfo, nullptr,
                        &_vkFrameBuffer));

    if (_vkFrameBuffer == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to create framebuffer({})", _renderTargetsInfo._debugName);
    }

#if defined(VK_DEBUG_NAME_ENABLED)
    DebugUtil::setObjectName(_logicalDevice, _vkFrameBuffer,
                             _renderTargetsInfo._debugName.c_str());
#endif

    return true;
}

void FrameBuffer::release()
{
    if (_vkFrameBuffer != VK_NULL_HANDLE)
    {
        vkDestroyFramebuffer(_logicalDevice->get(), _vkFrameBuffer, nullptr);
    }
}

}  // namespace VoxFlow
