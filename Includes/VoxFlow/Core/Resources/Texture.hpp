// Author : snowapril

#ifndef VOXEL_FLOW_TEXTURE_HPP
#define VOXEL_FLOW_TEXTURE_HPP

#include <vma/include/vk_mem_alloc.h>
#include <volk/volk.h>
#include <VoxFlow/Core/Resources/RenderResource.hpp>
#include <VoxFlow/Core/Resources/ResourceView.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string>
#include <string_view>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class TextureView;

class Texture final : public RenderResource
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

    // Get default created view that is pointing whole texture
    [[nodiscard]] inline TextureView* getDefaultView() const
    {
        return _defaultView;
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
    TextureView* _defaultView = nullptr;
};

class TextureView : public ResourceView
{
 public:
    explicit TextureView(std::string&& debugName, LogicalDevice* logicalDevice,
                         RenderResource* ownerResource);
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
    [[nodiscard]] inline TextureInfo getOwnerTextureInfo() const
    {
        return _ownerTextureInfo;
    }

    bool initialize(const TextureInfo& ownerTextureInfo, const TextureViewInfo& viewInfo);

    void release();

    VkDescriptorImageInfo getDescriptorImageInfo() const;

    ResourceViewType getResourceViewType() const final
    {
        return ResourceViewType::ImageView;
    }

 protected:
 private:
    VkImageView _vkImageView = VK_NULL_HANDLE;
    TextureInfo _ownerTextureInfo;
    TextureViewInfo _textureViewInfo;
};
}  // namespace VoxFlow

#endif