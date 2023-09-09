// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Resources/RenderResourceGarbageCollector.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
static VkImageUsageFlags convertToImageUsage(TextureUsage textureUsage)
{
    VkImageUsageFlags resultUsage = 0;
    if (static_cast<uint32_t>(textureUsage & TextureUsage::RenderTarget) > 0)
        resultUsage |= VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (static_cast<uint32_t>(textureUsage & TextureUsage::DepthStencil) > 0)
        resultUsage |= VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    if (static_cast<uint32_t>(textureUsage & TextureUsage::Sampled) > 0)
        resultUsage |= VK_IMAGE_USAGE_SAMPLED_BIT;
    if (static_cast<uint32_t>(textureUsage & TextureUsage::Storage) > 0)
        resultUsage |= VK_IMAGE_USAGE_STORAGE_BIT;
    if (static_cast<uint32_t>(textureUsage & TextureUsage::CopySrc) > 0)
        resultUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    if (static_cast<uint32_t>(textureUsage & TextureUsage::CopyDst) > 0)
        resultUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    return resultUsage;
}

static bool hasDepthAspect(VkFormat vkFormat)
{
    switch (vkFormat)
    {
        case VK_FORMAT_D16_UNORM:
        case VK_FORMAT_X8_D24_UNORM_PACK32:
        case VK_FORMAT_D32_SFLOAT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
    }
}

static bool hasStencilAspect(VkFormat vkFormat)
{
    switch (vkFormat)
    {
        case VK_FORMAT_S8_UINT:
        case VK_FORMAT_D16_UNORM_S8_UINT:
        case VK_FORMAT_D24_UNORM_S8_UINT:
        case VK_FORMAT_D32_SFLOAT_S8_UINT:
            return true;
        default:
            return false;
    }
}

Texture::Texture(std::string_view&& debugName, LogicalDevice* logicalDevice,
                 RenderResourceMemoryPool* renderResourceMemoryPool)
    : RenderResource(std::move(debugName), logicalDevice,
                     renderResourceMemoryPool)
{
}
Texture::~Texture()
{
    release();
}

bool Texture::makeAllocationResident(const TextureInfo& textureInfo)
{
    release();

    VOX_ASSERT(textureInfo._usage != TextureUsage::Unknown,
               "TextureUsage must be specified");

    // TODO(snowapril) : sample count, mipLevels, arrayLayers
    _textureInfo = textureInfo;
    VkImageCreateInfo imageCreateInfo{
        .sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .imageType = textureInfo._imageType,
        .format = textureInfo._format,
        .extent = VkExtent3D{ textureInfo._extent.x, textureInfo._extent.y,
                              textureInfo._extent.z },
        .mipLevels = 1,
        .arrayLayers = 1,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = convertToImageUsage(textureInfo._usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };

    VmaAllocationCreateInfo vmaInfo = {
        .flags =
            VMA_ALLOCATION_CREATE_STRATEGY_MIN_MEMORY_BIT,  // TODO(snowapril) :
                                                            // choose best one
        .usage = VMA_MEMORY_USAGE_AUTO,
        .requiredFlags = 0,
        .preferredFlags = 0,
        .memoryTypeBits = 0,
        .pool = VK_NULL_HANDLE,
        .pUserData = nullptr
    };

    VK_ASSERT(vmaCreateImage(_renderResourceMemoryPool->get(), &imageCreateInfo,
                             &vmaInfo, &_vkImage, &_allocation,
                             nullptr));

    if (_vkImage == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to initialize image({})", _debugName);
        return false;
    }

    _isSwapChainBackBuffer = false;

#if defined(VK_DEBUG_NAME_ENABLED)
    DebugUtil::setObjectName(_logicalDevice, _vkImage, _debugName.c_str());
#endif

    VkImageAspectFlags aspectFlag = 0;
    const bool hasDepth = hasDepthAspect(_textureInfo._format);
    const bool hasStencil = hasStencilAspect(_textureInfo._format);
    if (hasDepth)
    {
        aspectFlag |= VK_IMAGE_ASPECT_DEPTH_BIT;
    }
    if (hasStencil)
    {
        aspectFlag |= VK_IMAGE_ASPECT_STENCIL_BIT;
    }
    if ((hasDepth == false) && (hasStencil == false))
    {
        aspectFlag = VK_IMAGE_ASPECT_COLOR_BIT;
    }

    // TODO(snowapril) : need to handle cube map
    VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_1D;
    switch (_textureInfo._imageType)
    {
        case VK_IMAGE_TYPE_1D:
            imageViewType = textureInfo._extent.y > 1
                                ? VK_IMAGE_VIEW_TYPE_1D_ARRAY
                                : VK_IMAGE_VIEW_TYPE_1D;
            break;
        case VK_IMAGE_TYPE_2D:
            imageViewType = textureInfo._extent.z > 1
                                ? VK_IMAGE_VIEW_TYPE_2D_ARRAY
                                : VK_IMAGE_VIEW_TYPE_2D;
            break;        
        case VK_IMAGE_TYPE_3D:
            imageViewType = VK_IMAGE_VIEW_TYPE_3D;
            break;
    }

    std::optional<uint32_t> defaultViewIndex =
        createTextureView(TextureViewInfo{ ._viewType = imageViewType,
                                           ._format = _textureInfo._format,
                                           ._aspectFlags = aspectFlag,
                                           ._baseMipLevel = 0,
                                           ._levelCount = 1,
                                           ._baseArrayLayer = 0,
                                           ._layerCount = 1 });

    VOX_ASSERT(defaultViewIndex.has_value(),
               "Failed to create default texture view for texture({})",
               _debugName);

    if (defaultViewIndex.has_value())
    {
        _defaultView = getView(defaultViewIndex.value()).get();
    }

    return true;
}

bool Texture::initializeFromSwapChain(const TextureInfo& swapChainSurfaceInfo,
                                      VkImage swapChainImage)
{
    release();

    _textureInfo = swapChainSurfaceInfo;
    _vkImage = swapChainImage;

    _isSwapChainBackBuffer = true;

#if defined(VK_DEBUG_NAME_ENABLED)
    DebugUtil::setObjectName(_logicalDevice, _vkImage, _debugName.c_str());
#endif

    return true;
}

std::optional<uint32_t> Texture::createTextureView(
    const TextureViewInfo& viewInfo)
{
    const uint32_t viewIndex = static_cast<uint32_t>(_ownedTextureViews.size());
    std::shared_ptr<TextureView> textureView = std::make_shared<TextureView>(
        fmt::format("{}_View({})", _debugName, viewIndex), _logicalDevice,
        this);

    if (textureView->initialize(_textureInfo, viewInfo) == false)
    {
        return {};
    }

    _ownedTextureViews.push_back(std::move(textureView));
    return viewIndex;
}

void Texture::release()
{
    _ownedTextureViews.clear();
    if ((_isSwapChainBackBuffer == false) && (_vkImage != VK_NULL_HANDLE))
    {
        VmaAllocator vmaAllocator = _renderResourceMemoryPool->get();
        VkImage vkImage = _vkImage;
        VmaAllocation vmaAllocation = _allocation;
        RenderResourceGarbageCollector::Get().pushRenderResourceGarbage(
            RenderResourceGarbage(std::move(_accessedFences),
                                  [vmaAllocator, vkImage, vmaAllocation]() {
                                      vmaDestroyImage(vmaAllocator, vkImage,
                                                      vmaAllocation);
                                  }));

        _vkImage = VK_NULL_HANDLE;
        _allocation = VK_NULL_HANDLE;
    }
}

TextureView::TextureView(std::string&& debugName, LogicalDevice* logicalDevice,
                         RenderResource* ownerResource)
    : BindableResourceView(std::move(debugName), logicalDevice, ownerResource)
{
}

TextureView::~TextureView()
{
    release();
}

bool TextureView::initialize(const TextureInfo& ownerTextureInfo,
                             const TextureViewInfo& viewInfo)
{
    _ownerTextureInfo = ownerTextureInfo;
    _textureViewInfo = viewInfo;

    VkImageViewCreateInfo viewCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .image = static_cast<Texture*>(_ownerResource)->get(),
        .viewType = VK_IMAGE_VIEW_TYPE_2D,
        .format = viewInfo._format,
        .components = { VK_COMPONENT_SWIZZLE_R, VK_COMPONENT_SWIZZLE_G,
                        VK_COMPONENT_SWIZZLE_B, VK_COMPONENT_SWIZZLE_A },
        .subresourceRange = { .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                              .baseMipLevel = viewInfo._baseMipLevel,
                              .levelCount = viewInfo._levelCount,
                              .baseArrayLayer = viewInfo._baseArrayLayer,
                              .layerCount = viewInfo._layerCount }
    };

    VK_ASSERT(vkCreateImageView(_logicalDevice->get(), &viewCreateInfo, nullptr,
                                &_vkImageView));

    if (_vkImageView == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to create ImageView({})", _debugName);
        return false;
    }

#if defined(VK_DEBUG_NAME_ENABLED)
    DebugUtil::setObjectName(_logicalDevice, _vkImageView, _debugName.c_str());
#endif

    return true;
}

void TextureView::release()
{
    if (_vkImageView != VK_NULL_HANDLE)
    {
        VkDevice vkDevice = _logicalDevice->get();
        VkImageView vkImageView = _vkImageView;
        RenderResourceGarbageCollector::Get().pushRenderResourceGarbage(
            RenderResourceGarbage(
                std::move(_accessedFences), [vkDevice, vkImageView]() {
                    vkDestroyImageView(vkDevice, vkImageView, nullptr);
                }));

        _vkImageView = VK_NULL_HANDLE;
    }
}

VkDescriptorImageInfo TextureView::getDescriptorImageInfo() const
{
    return VkDescriptorImageInfo{
        .sampler = VK_NULL_HANDLE,
        .imageView = _vkImageView,
        .imageLayout = VK_IMAGE_LAYOUT_UNDEFINED,
    };
}

}  // namespace VoxFlow