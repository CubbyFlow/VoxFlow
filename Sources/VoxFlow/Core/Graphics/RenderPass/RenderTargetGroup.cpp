// Author : snowapril

#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderTargetGroup.hpp>
#include <algorithm>

namespace VoxFlow
{
bool RenderTargetLayoutKey::operator==(const RenderTargetLayoutKey& other) const
{
    return (_debugName == other._debugName) &&
           std::equal(_colorAttachmentDescs.begin(),
                      _colorAttachmentDescs.end(),
                      other._colorAttachmentDescs.begin(),
                      other._colorAttachmentDescs.end()) &&
           (_depthStencilAttachment == other._depthStencilAttachment);
}

bool RenderTargetsInfo::operator==(const RenderTargetsInfo& other) const
{
    return (_debugName == other._debugName) &&
           _vkRenderPass == other._vkRenderPass &&
           std::equal(_colorRenderTarget.begin(), _colorRenderTarget.end(),
                      other._colorRenderTarget.begin(),
                      other._colorRenderTarget.end()) &&
           (_depthStencilImage == other._depthStencilImage) &&
           (_resolution == other._resolution) && (_layers == other._layers) &&
           (_numSamples == other._numSamples);
}
};  // namespace VoxFlow

std::size_t std::hash<VoxFlow::RenderTargetLayoutKey>::operator()(
    VoxFlow::RenderTargetLayoutKey const& layoutKey) const noexcept
{
    uint32_t seed = 0;
    VoxFlow::hash_combine(seed, layoutKey._debugName);

    for (const auto& colorDesc : layoutKey._colorAttachmentDescs)
    {
        VoxFlow::hash_combine(seed, colorDesc._resolution.x);
        VoxFlow::hash_combine(seed, colorDesc._resolution.y);
        VoxFlow::hash_combine(seed, colorDesc._resolution.z);
        VoxFlow::hash_combine(seed, static_cast<uint32_t>(colorDesc._format));
        VoxFlow::hash_combine(seed, colorDesc._clearColor);
        VoxFlow::hash_combine(seed, colorDesc._clearColorValues.x);
        VoxFlow::hash_combine(seed, colorDesc._clearColorValues.y);
        VoxFlow::hash_combine(seed, colorDesc._clearColorValues.z);
        VoxFlow::hash_combine(seed, colorDesc._clearColorValues.w);
    }

    if (layoutKey._depthStencilAttachment.has_value())
    {
        const VoxFlow::DepthStencilPassDescription& depthStencilDesc =
            layoutKey._depthStencilAttachment.value();

        VoxFlow::hash_combine(seed, depthStencilDesc._resolution.x);
        VoxFlow::hash_combine(seed, depthStencilDesc._resolution.y);
        VoxFlow::hash_combine(seed, depthStencilDesc._resolution.z);
        VoxFlow::hash_combine(seed,
                              static_cast<uint32_t>(depthStencilDesc._format));
        VoxFlow::hash_combine(seed, depthStencilDesc._clearDepth);
        VoxFlow::hash_combine(seed, depthStencilDesc._clearStencil);
        VoxFlow::hash_combine(seed, depthStencilDesc._clearDepthValue);
        VoxFlow::hash_combine(seed, depthStencilDesc._clearStencilValue);
    }
    return seed;
}

std::size_t std::hash<VoxFlow::RenderTargetsInfo>::operator()(
    VoxFlow::RenderTargetsInfo const& rtInfo) const noexcept
{
    uint32_t seed = 0;
    VoxFlow::hash_combine(seed, rtInfo._debugName);
    VoxFlow::hash_combine(seed, reinterpret_cast<uint64_t>(rtInfo._vkRenderPass);
    for (const std::shared_ptr<VoxFlow::TextureView>& colorRT :
         rtInfo._colorRenderTarget)
    {
        VoxFlow::hash_combine(seed, reinterpret_cast<uint64_t>(colorRT->get()));
    }

    if (rtInfo._depthStencilImage.has_value())
    {
        const std::shared_ptr<VoxFlow::TextureView>& depthStencilImageView =
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