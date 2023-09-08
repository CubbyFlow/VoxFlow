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
#include <VoxFlow/Core/Utils/BitwiseOperators.hpp>
#include <memory>

namespace VoxFlow
{
class TextureView;

constexpr uint32_t BACK_BUFFER_COUNT = 3;
constexpr uint32_t FRAME_BUFFER_COUNT = 3;
constexpr uint32_t MAX_RENDER_TARGET_COUNTS = 8;
constexpr uint32_t MAX_ATTACHMENTS_COUNTS = MAX_RENDER_TARGET_COUNTS + 1;

enum class LogicalDeviceType : uint8_t
{
    MainDevice = 0,
    // SecondaryDevice = 1, TODO(snowapril) : support secondary device
    Undefined = 1,
    Count = Undefined
};

struct FrameContext
{
    uint32_t _swapChainIndex = UINT32_MAX;
    uint32_t _frameIndex = UINT32_MAX;
    uint32_t _backBufferIndex = UINT32_MAX;
};

enum class BufferUsage : uint32_t
{
    ConstantBuffer      = 0x00000001,
    RwStructuredBuffer  = 0x00000002,
    VertexBuffer        = 0x00000004,
    IndexBuffer         = 0x00000008,
    IndirectCommand     = 0x00000010,
    CopyDst             = 0x00000020,
    CopySrc             = 0x00000040,
    Readback            = 0x00000080,
    Upload              = 0x00000100,
    Unknown             = 0,
};
IMPL_BITWISE_OPERATORS(BufferUsage, uint32_t);

enum class ResourceAccessMask : uint32_t 
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
    StorageBuffer       = 0x00001000,
    UniformBuffer       = 0x00002000,
};
IMPL_BITWISE_OPERATORS(ResourceAccessMask, uint32_t);

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
IMPL_BITWISE_OPERATORS(TextureUsage, uint32_t);

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

#endif