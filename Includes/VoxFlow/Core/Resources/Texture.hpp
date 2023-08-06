// Author : snowapril

#ifndef VOXEL_FLOW_TEXTURE_HPP
#define VOXEL_FLOW_TEXTURE_HPP

#include <vma/include/vk_mem_alloc.h>
#include <volk/volk.h>
#include <VoxFlow/Core/Resources/RenderResource.hpp>
#include <VoxFlow/Core/Resources/BindableResourceView.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <string>
#include <string_view>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class TextureView;

class Texture final : public RenderResource, std::enable_shared_from_this<Texture>
{
 public:
    explicit Texture(std::string_view&& debugName, LogicalDevice* logicalDevice,
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

    [[nodiscard]] inline const TextureInfo& getTextureInfo() const
    {
        return _textureInfo;
    }

    [[nodiscard]] inline RenderResourceType getResourceType() const override
    {
        return RenderResourceType::Texture;
    }

    // Make the image allocation resident if evicted
    bool makeAllocationResident(const TextureInfo& textureInfo);

    // Create texture instance from swapchain image which should be separated
    // from others
    bool initializeFromSwapChain(const TextureInfo& swapChainSurfaceInfo,
                                 VkImage swapChainImage);

    // Create image view and return its index for given image view info
    std::optional<uint32_t> createTextureView(const TextureViewInfo& viewInfo);

    // Release image object to fence resource manager
    void release();

 protected:
 private:
    VkImage _vkImage = VK_NULL_HANDLE;
    TextureInfo _textureInfo;
    bool _isSwapChainBackBuffer = false;
    std::vector<std::shared_ptr<TextureView>> _ownedTextureViews;
};

class TextureView : public BindableResourceView
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

    VkDescriptorImageInfo getDescriptorImageInfo() const;

    ResourceViewType getResourceViewType() const final
    {
        return ResourceViewType::ImageView;
    }

 protected:
 private:
    std::weak_ptr<Texture> _ownerTexture;
    VkImageView _vkImageView = VK_NULL_HANDLE;
    TextureViewInfo _textureViewInfo;
};
}  // namespace VoxFlow

#endif