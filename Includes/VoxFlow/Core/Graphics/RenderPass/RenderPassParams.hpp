// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_PASS_PARAMS_HPP
#define VOXEL_FLOW_RENDER_PASS_PARAMS_HPP

#include <VoxFlow/Core/Utils/BitwiseOperators.hpp>
#include <VoxFlow/Core/Utils/HashUtil.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <array>
#include <glm/vec2.hpp>

namespace VoxFlow
{

enum class AttachmentMaskFlags : uint32_t
{
    None = 0x00000000,
    Color0 = 0x00000001,
    Color1 = 0x00000002,
    Color2 = 0x00000004,
    Color3 = 0x00000008,
    Color4 = 0x00000010,
    Color5 = 0x00000020,
    Color6 = 0x00000040,
    Color7 = 0x00000080,
    Depth = 0x00000100,
    Stencil = 0x00000200,
    DepthStencil = Depth | Stencil,
    All = DepthStencil | 0x000000ff,
};
IMPL_BITWISE_OPERATORS(AttachmentMaskFlags, uint32_t);

inline bool hasColorAspect(const AttachmentMaskFlags& attachmentFlags, const uint32_t index)
{
    return (static_cast<uint32_t>(attachmentFlags) & (static_cast<uint32_t>(AttachmentMaskFlags::Color0) << index)) > 0;
}

inline bool hasDepthAspect(const AttachmentMaskFlags& attachmentFlags)
{
    return (static_cast<uint32_t>(attachmentFlags) & static_cast<uint32_t>(AttachmentMaskFlags::Depth)) > 0;
}

inline bool hasStencilAspect(const AttachmentMaskFlags& attachmentFlags)
{
    return (static_cast<uint32_t>(attachmentFlags) & static_cast<uint32_t>(AttachmentMaskFlags::Stencil)) > 0;
}

struct RenderPassFlags
{
    AttachmentMaskFlags _clearFlags = AttachmentMaskFlags::None;
    AttachmentMaskFlags _loadFlags = AttachmentMaskFlags::All;
    AttachmentMaskFlags _storeFlags = AttachmentMaskFlags::All;

    inline bool operator==(const RenderPassFlags& rhs) const
    {
        return (_clearFlags == rhs._clearFlags) && (_loadFlags == rhs._loadFlags) && (_storeFlags == rhs._storeFlags);
    }
};

struct RenderPassParams
{
    RenderPassFlags _attachmentFlags;
    glm::uvec2 _viewportSize;
    std::array<glm::vec4, MAX_RENDER_TARGET_COUNTS> _clearColors;
    float _clearDepth = 0.0f;
    uint8_t _clearStencil = 0;
    AttachmentMaskFlags _writableAttachment = AttachmentMaskFlags::All;
};

}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::RenderPassFlags>
{
    std::size_t operator()(VoxFlow::RenderPassFlags const& passFlags) const noexcept;
};

#endif