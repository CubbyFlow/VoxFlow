// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_TARGET_GROUP_HPP
#define VOXEL_FLOW_RENDER_TARGET_GROUP_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassParams.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <array>
#include <string>

namespace VoxFlow
{

struct RenderTargetLayoutKey
{
    std::string _debugName;
    std::vector<VkFormat> _colorFormats;
    std::optional<VkFormat> _depthStencilFormat;
    RenderPassFlags _renderPassFlags;

    bool operator==(const RenderTargetLayoutKey& other) const;
};

struct RenderTargetsInfo
{
    std::string _debugName;
    VkRenderPass _vkRenderPass;
    std::vector<TextureView*> _colorRenderTarget;
    std::optional<TextureView*> _depthStencilImage;
    glm::uvec2 _resolution;
    uint8_t _layers = 0;
    uint8_t _numSamples = 0;

    bool operator==(const RenderTargetsInfo& other) const;
};

class Attachment
{
 public:
    Attachment() = default;
    explicit Attachment(TextureView* textureView) : _textureView(textureView)
    {
    }
    ~Attachment() = default;
    Attachment(const Attachment& rhs)
    {
        operator=(rhs);
    }
    Attachment& operator=(const Attachment& rhs)
    {
        if (this != &rhs)
        {
            _textureView = rhs._textureView;
        }
        return *this;
    }
    Attachment(Attachment&& rhs)
    {
        operator=(std::move(rhs));
    }
    Attachment& operator=(Attachment&& rhs)
    {
        if (this != &rhs)
        {
            _textureView = rhs._textureView;
            rhs._textureView = nullptr;
        }
        return *this;
    }

    [[nodiscard]] inline bool isValid() const
    {
        return _textureView != nullptr;
    }

    [[nodiscard]] inline TextureView* getView() const
    {
        return _textureView;
    }

 private:
    TextureView* _textureView = nullptr;
};

class AttachmentGroup
{
 public:
    AttachmentGroup() = default;
    explicit AttachmentGroup(std::vector<Attachment>&& colorAttachments, Attachment&& depthStencilAttachment, const uint32_t numSamples);
    ~AttachmentGroup() = default;
    AttachmentGroup(const AttachmentGroup& rhs)
    {
        operator=(rhs);
    }
    AttachmentGroup& operator=(const AttachmentGroup& rhs)
    {
        if (this != &rhs)
        {
            _colorAttachments = rhs._colorAttachments;
            _depthStencilAttachment = rhs._depthStencilAttachment;
            _numColorAttachments = rhs._numColorAttachments;
            _numSamples = rhs._numSamples;
        }
        return *this;
    }
    AttachmentGroup(AttachmentGroup&& rhs)
    {
        operator=(std::move(rhs));
    }
    AttachmentGroup& operator=(AttachmentGroup&& rhs)
    {
        if (this != &rhs)
        {
            _colorAttachments.swap(rhs._colorAttachments);
            _depthStencilAttachment = std::move(rhs._depthStencilAttachment);
            _numColorAttachments = rhs._numColorAttachments;
            _numSamples = rhs._numSamples;
        }
        return *this;
    }

    [[nodiscard]] inline Attachment getColor(const uint32_t index) const
    {
        return _colorAttachments[index];
    }

    [[nodiscard]] inline Attachment getDepthStencil() const
    {
        return _depthStencilAttachment;
    }

    [[nodiscard]] inline uint32_t getNumColorAttachments() const
    {
        return _numColorAttachments;
    }

    [[nodiscard]] inline bool hasDepthStencil() const
    {
        return _depthStencilAttachment.isValid();
    }

    [[nodiscard]] inline uint32_t getNumSamples() const
    {
        return _numSamples;
    }

 private:
    std::vector<Attachment> _colorAttachments;
    Attachment _depthStencilAttachment;
    uint32_t _numColorAttachments = 0;
    uint32_t _numSamples = 0;
};

}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::RenderTargetLayoutKey>
{
    std::size_t operator()(VoxFlow::RenderTargetLayoutKey const& layoutKey) const noexcept;
};

template <>
struct std::hash<VoxFlow::RenderTargetsInfo>
{
    std::size_t operator()(VoxFlow::RenderTargetsInfo const& rtInfo) const noexcept;
};

#endif