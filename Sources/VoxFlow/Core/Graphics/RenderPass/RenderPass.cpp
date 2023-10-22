// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
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
        static_cast<uint32_t>(rtLayoutKey._colorFormats.size());
    const bool hasDepthStencilAttachment =
        rtLayoutKey._depthStencilFormat.has_value();
    std::vector<VkAttachmentDescription> attachmentDescs;
    std::vector<VkAttachmentReference> colorAttachments;
    VkAttachmentReference depthAttachment;

    uint32_t attachmentIndex = 0;
    for (uint32_t i = 0; i < numColorAttachments; ++i)
    {
        const VkFormat vkColorFormat = rtLayoutKey._colorFormats[i];
        const bool loadColor =
            hasColorAspect(rtLayoutKey._renderPassFlags._loadFlags, i);
        const bool storeColor =
            hasColorAspect(rtLayoutKey._renderPassFlags._storeFlags, i);
        const bool clearColor =
            hasColorAspect(rtLayoutKey._renderPassFlags._clearFlags, i);

        VkAttachmentDescription colorAttachmentDesc{
            .flags = 0,
            .format = vkColorFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = clearColor
                          ? VK_ATTACHMENT_LOAD_OP_CLEAR
                          : (loadColor ? VK_ATTACHMENT_LOAD_OP_LOAD
                                       : VK_ATTACHMENT_LOAD_OP_DONT_CARE),
            .storeOp = storeColor ? VK_ATTACHMENT_STORE_OP_STORE
                                  : VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE,
            .stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
        };
        attachmentDescs.push_back(colorAttachmentDesc);

        VkAttachmentReference colorRef{
            .attachment = attachmentIndex++,
            .layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
        };
        colorAttachments.push_back(colorRef);
    }

    if (hasDepthStencilAttachment)
    {
        const VkFormat& vkDepthStencilFormat =
            _renderTargetLayout._depthStencilFormat.value();

        const bool loadDepth =
            hasDepthAspect(rtLayoutKey._renderPassFlags._loadFlags);
        const bool storeDepth =
            hasDepthAspect(rtLayoutKey._renderPassFlags._storeFlags);
        const bool clearDepth =
            hasDepthAspect(rtLayoutKey._renderPassFlags._clearFlags);

        const bool loadStencil =
            hasStencilAspect(rtLayoutKey._renderPassFlags._loadFlags);
        const bool storeStencil =
            hasStencilAspect(rtLayoutKey._renderPassFlags._storeFlags);
        const bool clearStencil =
            hasStencilAspect(rtLayoutKey._renderPassFlags._clearFlags);

        VkAttachmentDescription depthAttachmentDesc
        {
            .flags = 0, .format = vkDepthStencilFormat,
            .samples = VK_SAMPLE_COUNT_1_BIT,
            .loadOp = clearDepth
                          ? VK_ATTACHMENT_LOAD_OP_CLEAR
                          : (loadDepth ? VK_ATTACHMENT_LOAD_OP_LOAD
                                       : VK_ATTACHMENT_LOAD_OP_DONT_CARE),
            .storeOp = storeDepth ? VK_ATTACHMENT_STORE_OP_STORE
                                  : VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .stencilLoadOp = clearStencil
                                 ? VK_ATTACHMENT_LOAD_OP_CLEAR
                                 : (loadStencil ? VK_ATTACHMENT_LOAD_OP_LOAD : VK_ATTACHMENT_LOAD_OP_DONT_CARE),
            .stencilStoreOp = storeStencil ? VK_ATTACHMENT_STORE_OP_DONT_CARE : VK_ATTACHMENT_STORE_OP_DONT_CARE,
            .initialLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
            .finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
        };
        attachmentDescs.push_back(depthAttachmentDesc);

        depthAttachment = VkAttachmentReference{
            .attachment = attachmentIndex++,
            .layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
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

    // TODO(snowapril) : support multi-pass architecture. for now, use only
    // single pass and self-dependency
    std::vector<VkSubpassDependency> dependencies = {
        /* VkSubpassDependency */ {
            .srcSubpass = VK_SUBPASS_EXTERNAL,
            .dstSubpass = 0,
            .srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            .srcAccessMask = 0,
            .dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                             VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT,
            .dependencyFlags = 0 },
    };

    const VkRenderPassCreateInfo renderPassInfo = {
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

    [[maybe_unused]] VkResult result = vkCreateRenderPass(
        _logicalDevice->get(), &renderPassInfo, nullptr, &_renderPass);

    VK_ASSERT(result);

    if (_renderPass == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, " Failed to create RenderPass({})",
                   _renderTargetLayout._debugName);
        return false;
    }

#if defined(VK_DEBUG_NAME_ENABLED)
    const std::string renderPassDebugName =
        fmt::format("{}_Color(#{})_Depth(#{})", _renderTargetLayout._debugName,
                    numColorAttachments, hasDepthStencilAttachment ? 1U : 0U);
    DebugUtil::setObjectName(_logicalDevice, _renderPass,
                             renderPassDebugName.c_str());
#endif

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