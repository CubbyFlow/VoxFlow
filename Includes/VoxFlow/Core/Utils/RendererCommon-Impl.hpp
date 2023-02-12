// Author : snowapril

#ifndef VOXEL_FLOW_RENDERER_COMMON_IMPL_HPP
#define VOXEL_FLOW_RENDERER_COMMON_IMPL_HPP

#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
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
           std::equal(_colorRenderTarget.begin(), _colorRenderTarget.end(),
                      other._colorRenderTarget.begin(),
                      other._colorRenderTarget.end()) &&
           (_depthStencilImage == other._depthStencilImage) &&
           (_resolution == other._resolution);
}
}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::RenderTargetLayoutKey>
{
    std::size_t operator()(
        VoxFlow::RenderTargetLayoutKey const& layoutKey) const noexcept
    {
        uint32_t seed = 0;
        VoxFlow::hash_combine(seed, layoutKey._debugName);

        for (const auto& colorDesc : layoutKey._colorAttachmentDescs)
        {
            VoxFlow::hash_combine(seed, colorDesc._resolution.x);
            VoxFlow::hash_combine(seed, colorDesc._resolution.y);
            VoxFlow::hash_combine(seed, colorDesc._resolution.z);
            VoxFlow::hash_combine(seed,
                                  static_cast<uint32_t>(colorDesc._format));
            VoxFlow::hash_combine(seed, colorDesc._clearColor);
        }

        if (layoutKey._depthStencilAttachment.has_value())
        {
            const VoxFlow::DepthStencilPassDescription& depthStencilDesc =
                layoutKey._depthStencilAttachment.value();

            VoxFlow::hash_combine(seed, depthStencilDesc._resolution.x);
            VoxFlow::hash_combine(seed, depthStencilDesc._resolution.y);
            VoxFlow::hash_combine(seed, depthStencilDesc._resolution.z);
            VoxFlow::hash_combine(
                seed, static_cast<uint32_t>(depthStencilDesc._format));
            VoxFlow::hash_combine(seed, depthStencilDesc._clearDepth);
            VoxFlow::hash_combine(seed, depthStencilDesc._clearStencil);
        }
        return seed;
    }
};

template <>
struct std::hash<VoxFlow::RenderTargetsInfo>
{
    std::size_t operator()(
        VoxFlow::RenderTargetsInfo const& rtInfo) const noexcept
    {
        uint32_t seed = 0;
        VoxFlow::hash_combine(seed, rtInfo._debugName);
        for (const std::shared_ptr<VoxFlow::Texture>& colorRT :
             rtInfo._colorRenderTarget)
        {
            VoxFlow::hash_combine(seed, reinterpret_cast<uint64_t>(colorRT->get()));
        }

        if (rtInfo._depthStencilImage.has_value())
        {
            const std::shared_ptr<VoxFlow::Texture>& depthStencilImage =
                rtInfo._depthStencilImage.value();

            VoxFlow::hash_combine(
                seed, reinterpret_cast<uint64_t>(depthStencilImage->get()));
        }

        VoxFlow::hash_combine(seed, rtInfo._resolution.x);
        VoxFlow::hash_combine(seed, rtInfo._resolution.y);

        return seed;
    }
};

#endif