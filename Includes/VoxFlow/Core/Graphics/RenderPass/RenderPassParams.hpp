// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_PASS_PARAMS_HPP
#define VOXEL_FLOW_RENDER_PASS_PARAMS_HPP

#include <VoxFlow/Core/Utils/BitwiseOperators.hpp>
#include <array>

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

struct RenderPassFlags
{
    AttachmentMaskFlags _clearFlags = AttachmentMaskFlags::None;
    AttachmentMaskFlags _loadFlags = AttachmentMaskFlags::All;
    AttachmentMaskFlags _storeFlags = AttachmentMaskFlags::All;
};

struct RenderPassParams
{
    std::array<RenderPassFlags, MAX_RENDER_TARGET_COUNTS> _attachmentFlags;
    glm::uvec2 _viewportSize;
    std::array<glm::vec4, MAX_RENDER_TARGET_COUNTS> _clearColors;
    float _clearDepth = 0.0f;
    uint8_t _clearStencil = 0;
    AttachmentMaskFlags _writableAttachment = AttachmentMaskFlags::All;
};

}  // namespace VoxFlow

#endif