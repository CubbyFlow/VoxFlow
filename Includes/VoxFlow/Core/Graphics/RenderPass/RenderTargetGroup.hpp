// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_TARGET_GROUP_HPP
#define VOXEL_FLOW_RENDER_TARGET_GROUP_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Utils/Handle.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <array>
#include <string>

namespace VoxFlow
{

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

    inline bool operator==(const DepthStencilPassDescription& other) const
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
    VkRenderPass _vkRenderPass;
    std::vector<Handle<TextureView>> _colorRenderTarget;
    std::optional<Handle<TextureView>> _depthStencilImage;
    glm::uvec2 _resolution;
    uint8_t _layers = 0;
    uint8_t _numSamples = 0;

    bool operator==(const RenderTargetsInfo& other) const;
};

class Attachment
{
 public:
    explicit Attachment(Handle<Texture>&& textureHandle);
    ~Attachment();

 private:
    Handle<Texture> _textureHandle;
};

class AttachmentGroup
{
 public:
    AttachmentGroup();
    ~AttachmentGroup();

 private:
    std::array<Attachment, MAX_RENDER_TARGET_COUNTS> _colorAttachments;
    Attachment _depthStencilAttachment;
};

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