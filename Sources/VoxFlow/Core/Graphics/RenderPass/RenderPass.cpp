// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/RenderPass//RenderPass.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>

namespace VoxFlow
{
RenderPass::RenderPass(const std::shared_ptr<LogicalDevice>& device)
    : _device(device)
{
    auto subpassDesc = Initializer::MakeInfo<VkSubpassDescription>();

    [[maybe_unused]] const VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = 0,
        .pAttachments = nullptr,
        .subpassCount = 1,
        .pSubpasses = &subpassDesc,
        .dependencyCount = 0,
        .pDependencies = nullptr
    };

    // TODO(snowapril) : add attachment, subpass, dependency parameters
    // TODO(snowapril) : deploy render graphy system
    VK_ASSERT(vkCreateRenderPass(_device->get(), &renderPassInfo, nullptr,
                                 &_renderPass));
}

RenderPass::~RenderPass()
{
    release();
}

RenderPass::RenderPass(RenderPass&& other) noexcept
    : _device(std::move(other._device)), _renderPass(other._renderPass)
{
    // Do nothing
}

RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
{
    if (&other != this)
    {
        _device = std::move(other._device);
        _renderPass = other._renderPass;
    }
    return *this;
}

void RenderPass::release()
{
    if (_renderPass)
    {
        vkDestroyRenderPass(_device->get(), _renderPass, nullptr);
        _renderPass = VK_NULL_HANDLE;
    }
    _device.reset();
}
}  // namespace VoxFlow