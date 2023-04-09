// Author : snowapril

#ifndef VOXEL_FLOW_TEXTURE_HPP
#define VOXEL_FLOW_TEXTURE_HPP

#include <volk/volk.h>
#include <vma/include/vk_mem_alloc.h>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <string>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class TextureView;

class Texture : private NonCopyable, public std::enable_shared_from_this<Texture>
{
 public:
    explicit Texture(std::string&& debugName, LogicalDevice* logicalDevice,
                     RenderResourceMemoryPool* renderResourceMemoryPool);
    ~Texture();

 public:
    [[nodiscard]] inline VkImage get() const
    {
        return _vkImage;
    }
    [[nodiscard]] inline std::shared_ptr<TextureView> getView(
        const uint32_t viewIndex) const
    {
        VOX_ASSERT(viewIndex < static_cast<uint32_t>(_ownedTextureViews.size()),
                   "Given Index ({}), Num Image Views ({})", viewIndex,
                   _ownedTextureViews.size());
        return _ownedTextureViews[viewIndex];
    }

    bool initialize(const TextureInfo& textureInfo);
    bool initializeFromSwapChain(const TextureInfo& swapChainSurfaceInfo,
                                 VkImage swapChainImage);

    std::optional<uint32_t> createTextureView(const TextureViewInfo& viewInfo);

    void release();

 protected:
 private:
    std::string _debugName;
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool = nullptr;
    VkImage _vkImage = VK_NULL_HANDLE;
    VmaAllocation _textureAllocation = nullptr;
    TextureInfo _textureInfo;
    bool _isFromSwapChain = false;

    std::vector<std::shared_ptr<TextureView>> _ownedTextureViews;
};

class TextureView : private NonCopyable
{
 public:
    explicit TextureView(std::string&& debugName, LogicalDevice* logicalDevice,
                         std::weak_ptr<Texture>&& ownerTexture);
    ~TextureView();

 public:
    [[nodiscard]] inline VkImageView get() const
    {
        return _vkImageView;
    }
    [[nodiscard]] inline TextureViewInfo getViewInfo() const
    {
        return _textureViewInfo;
    }

    bool initialize(const TextureViewInfo& viewInfo);

    void release();

 protected:
 private:
    std::string _debugName;
    LogicalDevice* _logicalDevice = nullptr;
    std::weak_ptr<Texture> _ownerTexture;
    VkImageView _vkImageView = VK_NULL_HANDLE;
    TextureViewInfo _textureViewInfo;
};
}  // namespace VoxFlow

#endif