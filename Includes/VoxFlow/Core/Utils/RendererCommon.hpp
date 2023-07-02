// Author : snowapril

#ifndef VOXEL_FLOW_RENDERER_COMMON_HPP
#define VOXEL_FLOW_RENDERER_COMMON_HPP

#include <volk/volk.h>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <VoxFlow/Core/Utils/HashUtil.hpp>
#include <vector>
#include <string>
#include <optional>
#include <functional>
#include <memory>

namespace VoxFlow
{
class TextureView;

constexpr uint32_t BACK_BUFFER_COUNT = 3;
constexpr uint32_t FRAME_BUFFER_COUNT = 2;
constexpr uint32_t MAX_RENDER_TARGET_COUNTS = 8;

enum class SetSlotCategory : uint8_t
{
    Bindless = 0,
    PerFrame = 1,
    PerRenderPass = 2,
    PerDraw = 3,
    Count = 4,
};
constexpr uint32_t MAX_NUM_SET_SLOTS =
    static_cast<uint32_t>(SetSlotCategory::Count);

enum class CommandBufferUsage : uint8_t 
{
    Graphics = 0,
    Compute = 1,
    Transfer = 2,
    Count = 4,
    Undefined = 5,
};

enum class BufferUsage : uint32_t
{
    ConstantBuffer      = 0x00000001,
    RwStructuredBuffer  = 0x00000010,
    VertexBuffer        = 0x00000100,
    IndexBuffer         = 0x00001000,
    IndirectCommand     = 0x00010000,
    CopyDst             = 0x00100000,
    CopySrc             = 0x01000000,
    Unknown             = 0,
};

enum class ResourceLayout : uint32_t 
{
    Undefined           = 0x00000001,
    TransferSource      = 0x00000002,
    TransferDest        = 0x00000004,
    VertexBuffer        = 0x00000008,
    IndexBuffer         = 0x00000010,
    ColorAttachment     = 0x00000020,
    DepthAttachment     = 0x00000040,
    StencilAttachment   = 0x00000080,
    DepthReadOnly       = 0x00000100,
    StencilReadOnly     = 0x00000200,
    ShaderReadOnly      = 0x00000400,
    General             = 0x00000800,
};

inline uint32_t operator|(BufferUsage lhs, BufferUsage rhs)
{
    return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

inline uint32_t operator&(BufferUsage lhs, BufferUsage rhs)
{
    return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
}

struct BufferInfo
{
    uint64_t _size = 0;
    BufferUsage _usage = BufferUsage::Unknown;
};

struct BufferViewInfo
{
    VkFormat _format = VK_FORMAT_UNDEFINED;
    uint64_t _offset = 0;
    uint64_t _range = 0;
};

enum class TextureUsage : uint32_t
{
    RenderTarget    = 0x00000001,
    DepthStencil    = 0x00000010,
    Sampled         = 0x00000100,
    Storage         = 0x00001000,
    CopySrc         = 0x00010000,
    CopyDst         = 0x00100000,
    Unknown         = 0,
};

inline uint32_t operator|(TextureUsage lhs, TextureUsage rhs)
{
    return static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs);
}

inline uint32_t operator&(TextureUsage lhs, TextureUsage rhs)
{
    return static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs);
}

struct TextureInfo
{
    glm::uvec3 _extent{ 0, 0, 0 };
    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkImageType _imageType = VK_IMAGE_TYPE_2D;
    TextureUsage _usage = TextureUsage::Unknown;
};

struct TextureViewInfo
{
    VkImageViewType _viewType = VK_IMAGE_VIEW_TYPE_2D;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    VkImageAspectFlags _aspectFlags = 0;
    uint32_t _baseMipLevel = 0;
    uint32_t _levelCount = 1;
    uint32_t _baseArrayLayer = 0;
    uint32_t _layerCount = 1;
};

struct ColorPassDescription
{
    glm::ivec3 _resolution;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    bool _clearColor = false;
    glm::vec4 _clearColorValues;

    inline bool operator==(const ColorPassDescription& other) const
    {
        return (_resolution == other._resolution) &&
               (_format == other._format) &&
               (_clearColor == other._clearColor) &&
               (_clearColorValues == other._clearColorValues);
    }
};

struct DepthStencilPassDescription
{
    glm::ivec3 _resolution;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    bool _clearDepth = false;
    bool _clearStencil = false;
    float _clearDepthValue = 0.0f;
    uint32_t _clearStencilValue = 0;

    inline bool operator==(
        const DepthStencilPassDescription& other) const
    {
        return (_resolution == other._resolution) &&
               (_format == other._format) &&
               (_clearDepth == other._clearDepth) &&
               (_clearStencil == other._clearStencil) &&
               (_clearDepthValue == other._clearDepthValue) &&
               (_clearStencilValue == other._clearStencilValue);
    }
};

struct RenderTargetLayoutKey
{
    std::string _debugName;
    std::vector<ColorPassDescription> _colorAttachmentDescs;
    std::optional<DepthStencilPassDescription> _depthStencilAttachment;

    bool operator==(const RenderTargetLayoutKey& other) const;
};

struct RenderTargetsInfo
{
    std::string _debugName;
    RenderTargetLayoutKey _layoutKey;
    std::vector<std::shared_ptr<TextureView>> _colorRenderTarget;
    std::optional<std::shared_ptr<TextureView>> _depthStencilImage;
    glm::uvec2 _resolution;

    bool operator==(const RenderTargetsInfo& other) const;
};

// Below helper types from
// https://en.cppreference.com/w/cpp/utility/variant/visit
template <class... Ts>
struct overloaded : Ts...
{
    using Ts::operator()...;
};
// explicit deduction guide (not needed as of C++20)
template <class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::RenderTargetLayoutKey>
{
    std::size_t operator()(
        VoxFlow::RenderTargetLayoutKey const& layoutKey) const noexcept;
};

template <>
struct std::hash<VoxFlow::RenderTargetsInfo>
{
    std::size_t operator()(
        VoxFlow::RenderTargetsInfo const& rtInfo) const noexcept;
};

#endif