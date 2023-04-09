// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
RenderPass::RenderPass(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
}

RenderPass::~RenderPass()
{
    release();
}

RenderPass::RenderPass(RenderPass&& other) noexcept
{
    operator=(std::move(other));
}

RenderPass& RenderPass::operator=(RenderPass&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = other._logicalDevice;
        _renderPass = other._renderPass;
    }
    return *this;
}

bool RenderPass::initialize(const RenderTargetLayoutKey& rtLayoutKey)
{
    _renderTargetLayout = rtLayoutKey;

    const uint32_t numColorAttachments =
        static_cast<uint32_t>(rtLayoutKey._colorAttachmentDescs.size());
    const bool hasDepthStencilAttachment =
        rtLayoutKey._depthStencilAttachment.has_value();
    std::vector<VkAttachmentDescription> attachmentDescs;
    std::vector<VkAttachmentReference> colorAttachments;
    VkAttachmentReference depthAttachment;

    uint32_t attachmentIndex = 0;
    for (const ColorPassDescription& colorDesc :
         _renderTargetLayout._colorAttachmentDescs)
    {
        VkAttachmentDescription colorAttachmentDesc{
            .flags = 0,
            .format = colorDesc._format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = colorDesc._clearColor ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                            : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout =
                colorDesc._clearColor ? VK_IMAGE_LAYOUT_UNDEFINED : VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        attachmentDescs.push_back(colorAttachmentDesc);

        VkAttachmentReference colorRef
        {
            .attachment = attachmentIndex++, 
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };
        colorAttachments.push_back(colorRef);
    }

    if (hasDepthStencilAttachment)
    {
        const DepthStencilPassDescription& depthStencilDesc =
            _renderTargetLayout._depthStencilAttachment.value();
        VkAttachmentDescription depthAttachmentDesc{
            .flags = 0,
            .format = depthStencilDesc._format,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = depthStencilDesc._clearDepth ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                                   : VK_ATTACHMENT_LOAD_OP_LOAD,
            .storeOp = VK_ATTACHMENT_STORE_OP_STORE,
            .stencilLoadOp = depthStencilDesc._clearStencil
                                 ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                 : VK_ATTACHMENT_LOAD_OP_LOAD,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };
        attachmentDescs.push_back(depthAttachmentDesc);

        depthAttachment = VkAttachmentReference{
            .attachment = attachmentIndex++,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };
    }

    VkSubpassDescription subPassDesc{
        .flags = 0,
        .pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS,
        .inputAttachmentCount = 0,
        .pInputAttachments = nullptr,
        .colorAttachmentCount = numColorAttachments,
        .pColorAttachments = colorAttachments.data(),
        .pResolveAttachments = nullptr,
        .pDepthStencilAttachment =
            hasDepthStencilAttachment ? &depthAttachment : nullptr,
        .preserveAttachmentCount = 0,
        .pPreserveAttachments = nullptr
    };
    
    std::vector<VkSubpassDependency> dependencies = {
        /* VkSubpassDependency */ {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = 0 }
    };

    [[maybe_unused]] const VkRenderPassCreateInfo renderPassInfo = {
        .sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .attachmentCount = static_cast<uint32_t>(attachmentDescs.size()),
        .pAttachments = attachmentDescs.data(),
        .subpassCount = 1,
        .pSubpasses = &subPassDesc,
        .dependencyCount = static_cast<uint32_t>(dependencies.size()),
        .pDependencies = dependencies.data()
    };

    VK_ASSERT(vkCreateRenderPass(_logicalDevice->get(), &renderPassInfo,
                                 nullptr, &_renderPass));

    if (_renderPass == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, " Failed to create RenderPass({})",
                   _renderTargetLayout._debugName);
        return false;
    }

    const std::string renderPassDebugName =
        fmt::format("{}_Color(#{})_Depth(#{})", _renderTargetLayout._debugName,
                    numColorAttachments, hasDepthStencilAttachment ? 1U : 0U);
    DebugUtil::setObjectName(_logicalDevice, _renderPass,
                             renderPassDebugName.c_str());
    return true;
}

void RenderPass::release()
{
    if (_renderPass)
    {
        vkDestroyRenderPass(_logicalDevice->get(), _renderPass, nullptr);
        _renderPass = VK_NULL_HANDLE;
    }
}
}  // namespace VoxFlow