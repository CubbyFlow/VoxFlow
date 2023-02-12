// Author : snowapril

#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>

namespace VoxFlow
{
static VkImageUsageFlags convertToImageUsage(TextureUsage textureUsage)
{
    VkImageUsageFlags resultUsage = 0;
    if (textureUsage & TextureUsage::RenderTarget)
        resultUsage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
    if (textureUsage & TextureUsage::DepthStencil)
        resultUsage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
    if (textureUsage & TextureUsage::Sampled)
        resultUsage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
    if (textureUsage & TextureUsage::Storage)
        resultUsage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT;
    if (textureUsage & TextureUsage::CopySrc)
        resultUsage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
    if (textureUsage & TextureUsage::CopyDst)
        resultUsage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
    return resultUsage;
}

Texture::Texture(std::string&& debugName, LogicalDevice* logicalDevice,
                 RenderResourceMemoryPool* renderResourceMemoryPool)
    : _debugName(std::move(debugName)),
      _logicalDevice(logicalDevice),
      _renderResourceMemoryPool(renderResourceMemoryPool)
{
}
Texture::~Texture()
{
}

bool Texture::initialize(TextureInfo textureInfo)
{
    VOX_ASSERT(textureInfo._usage != TextureUsage::Unknown,
               "TextureUsage must be specified");

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
        .arrayLayers = 0,
        .samples = VK_SAMPLE_COUNT_1_BIT,
        .tiling = VK_IMAGE_TILING_OPTIMAL,
        .usage = convertToImageUsage(textureInfo._usage),
        .sharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .initialLayout = VK_IMAGE_LAYOUT_GENERAL,
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
                             &vmaInfo, &_vkImage, &_textureAllocation, nullptr));

    if (_vkImage == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to initialize image(%s)", _debugName);
        return false;
    }

    DebugUtil::setObjectName(_logicalDevice, _vkImage, _debugName.c_str());
    return true;
}

void Texture::release()
{
    if (_vkImage != VK_NULL_HANDLE)
    {
        vmaDestroyImage(_renderResourceMemoryPool->get(), _vkImage,
                        _textureAllocation);    
    }
}

}  // namespace VoxFlow