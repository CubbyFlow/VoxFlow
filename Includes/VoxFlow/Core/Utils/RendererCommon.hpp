// Author : snowapril

#ifndef VOXEL_FLOW_RENDERER_COMMON_HPP
#define VOXEL_FLOW_RENDERER_COMMON_HPP

#include <volk/volk.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <VoxFlow/Core/Utils/HashUtil.hpp>
#include <vector>
#include <string>
#include <optional>
#include <functional>

namespace VoxFlow
{
class Texture;

constexpr uint32_t BACK_BUFFER_COUNT = 2;
constexpr uint32_t FRAME_BUFFER_COUNT = 2;

enum DescriptorSetCycle : uint8_t
{
    GLOBAL          = 0,
    RENDERPASS      = 1,
    PER_DRAW        = 2,
    COUNT           = 3,
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
    uint32_t _size = 0;
    BufferUsage _usage = BufferUsage::Unknown;
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

struct ColorPassDescription
{
    glm::ivec3 _resolution;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    bool _clearColor = false;

    inline bool operator==(const ColorPassDescription& other) const
    {
        return (_resolution == other._resolution) &&
               (_format == other._format) && (_clearColor == other._clearColor);
    }
};

struct DepthStencilPassDescription
{
    glm::ivec3 _resolution;
    VkFormat _format = VK_FORMAT_UNDEFINED;
    bool _clearDepth = false;
    bool _clearStencil = false;

    inline bool operator==(
        const DepthStencilPassDescription& other) const
    {
        return (_resolution == other._resolution) &&
               (_format == other._format) &&
               (_clearDepth == other._clearDepth) &&
               (_clearStencil == other._clearStencil);
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
    std::vector<std::shared_ptr<Texture>> _colorRenderTarget;
    std::optional<std::shared_ptr<Texture>> _depthStencilImage;
    glm::ivec2 _resolution;

    bool operator==(const RenderTargetsInfo& other) const;
};

}  // namespace VoxFlow

#endif