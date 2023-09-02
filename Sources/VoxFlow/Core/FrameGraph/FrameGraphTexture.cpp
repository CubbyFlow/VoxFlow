// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp>
#include <VoxFlow/Core/Resources/RenderResourceAllocator.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{

namespace RenderGraph
{
bool FrameGraphTexture::create(RenderResourceAllocator* resourceAllocator,
                               std::string&& debugName, Descriptor descriptor,
                               Usage usage)
{
    // TODO(snowapril) : 

    VkImageType imageType = VK_IMAGE_TYPE_MAX_ENUM;
    VkImageViewType imageViewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
    if (descriptor._depth > 1)
    {
        imageType = VK_IMAGE_TYPE_3D;
        imageViewType = VK_IMAGE_VIEW_TYPE_3D;
    }
    else if (descriptor._height > 1)
    {
        imageType = VK_IMAGE_TYPE_2D;
        imageViewType = VK_IMAGE_VIEW_TYPE_2D;
    }
    else
    {
        imageType = VK_IMAGE_TYPE_1D;
        imageViewType = VK_IMAGE_VIEW_TYPE_1D;
    }

    _texture = resourceAllocator->allocateTexture(
        TextureInfo{ ._extent =
                         glm::uvec3(descriptor._width, descriptor._height,
                                    descriptor._depth),
                     ._format = descriptor._format,
                     ._imageType = imageType,
                     ._usage = usage },
        std::move(debugName));

    uint32_t viewIndex = _texture
                             ->createTextureView(TextureViewInfo{
                                 ._viewType = imageViewType,
                                 ._format = descriptor._format,
                                 ._aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
                                 ._baseMipLevel = descriptor._level,
                                 ._levelCount = 1,
                                 ._baseArrayLayer = 0,
                                 ._layerCount = 1 })
                             .value();

    _textureView = _texture->getView(viewIndex).get();

    return true;
}

void FrameGraphTexture::destroy(RenderResourceAllocator* resourceAllocator)
{
    (void)resourceAllocator;
}
}  // namespace RenderGraph

}  // namespace VoxFlow