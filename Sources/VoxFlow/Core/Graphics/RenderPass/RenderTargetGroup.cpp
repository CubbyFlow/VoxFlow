// Author : snowapril

#include <VoxFlow/Core/Graphics/RenderPass/RenderTargetGroup.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <algorithm>

namespace VoxFlow
{
bool RenderTargetLayoutKey::operator==(const RenderTargetLayoutKey& other) const
{
    return std::equal(_colorFormats.begin(), _colorFormats.end(),
                      other._colorFormats.begin(), other._colorFormats.end()) &&
           (_depthStencilFormat == other._depthStencilFormat) &&
           (_renderPassFlags == other._renderPassFlags) &&
           (_debugName == other._debugName);
}

bool RenderTargetsInfo::operator==(const RenderTargetsInfo& other) const
{
    return _vkRenderPass == other._vkRenderPass &&
           std::equal(_colorRenderTarget.begin(), _colorRenderTarget.end(),
                      other._colorRenderTarget.begin(),
                      other._colorRenderTarget.end()) &&
           (_depthStencilImage == other._depthStencilImage) &&
           (_resolution == other._resolution) && (_layers == other._layers) &&
           (_numSamples == other._numSamples) &&
           (_debugName == other._debugName);
}

AttachmentGroup::AttachmentGroup(std::vector<Attachment>&& colorAttachments,
                                 Attachment&& depthStencilAttachment,
                                 const uint32_t numSamples)
    : _colorAttachments(std::move(colorAttachments)),
      _depthStencilAttachment(std::move(depthStencilAttachment)),
      _numColorAttachments(static_cast<uint32_t>(_colorAttachments.size())),
      _numSamples(numSamples)
{
}

};  // namespace VoxFlow

std::size_t std::hash<VoxFlow::RenderTargetLayoutKey>::operator()(
    VoxFlow::RenderTargetLayoutKey const& layoutKey) const noexcept
{
    uint32_t seed = 0;

    VoxFlow::hash_combine(seed, layoutKey._debugName);
    for (const auto& colorFormat : layoutKey._colorFormats)
    {
        VoxFlow::hash_combine(seed, static_cast<uint32_t>(colorFormat));
    }

    if (layoutKey._depthStencilFormat.has_value())
    {
        const VkFormat vkDepthStencilFormat =
            layoutKey._depthStencilFormat.value();

        VoxFlow::hash_combine(seed,
                              static_cast<uint32_t>(vkDepthStencilFormat));
    }

    VoxFlow::hash_combine(seed, layoutKey._renderPassFlags);

    return seed;
}

std::size_t std::hash<VoxFlow::RenderTargetsInfo>::operator()(
    VoxFlow::RenderTargetsInfo const& rtInfo) const noexcept
{
    uint32_t seed = 0;

    VoxFlow::hash_combine(seed, rtInfo._debugName);
    VoxFlow::hash_combine(seed,
                          reinterpret_cast<uint64_t>(rtInfo._vkRenderPass));
    for (VoxFlow::TextureView* colorRT : rtInfo._colorRenderTarget)
    {
        VoxFlow::hash_combine(seed, reinterpret_cast<uint64_t>(colorRT->get()));
    }

    if (rtInfo._depthStencilImage.has_value())
    {
        VoxFlow::TextureView* depthStencilImageView =
            rtInfo._depthStencilImage.value();

        VoxFlow::hash_combine(
            seed, reinterpret_cast<uint64_t>(depthStencilImageView->get()));
    }

    VoxFlow::hash_combine(seed, rtInfo._resolution.x);
    VoxFlow::hash_combine(seed, rtInfo._resolution.y);
    VoxFlow::hash_combine(seed, rtInfo._layers);
    VoxFlow::hash_combine(seed, rtInfo._numSamples);

    return seed;
}