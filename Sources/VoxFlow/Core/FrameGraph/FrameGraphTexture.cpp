// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp>
#include <VoxFlow/Core/Resources/RenderResourceAllocator.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{

namespace RenderGraph
{
bool FrameGraphTexture::create(RenderResourceAllocator* resourceAllocator, std::string&& debugName, Descriptor descriptor, Usage usage)
{
    // TODO(snowapril) :

    const glm::uvec3 extent(descriptor._width, descriptor._height, descriptor._depth);
    const VkImageType imageType = convertToImageType(extent);
    const VkImageViewType imageViewType = convertToImageViewType(imageType, extent);

    _texture = resourceAllocator->allocateTexture(TextureInfo{ ._extent = glm::uvec3(descriptor._width, descriptor._height, descriptor._depth),
                                                               ._format = descriptor._format,
                                                               ._imageType = imageType,
                                                               ._usage = usage },
                                                  std::move(debugName));

    uint32_t viewIndex = _texture
                             ->createTextureView(TextureViewInfo{ ._viewType = imageViewType,
                                                                  ._format = descriptor._format,
                                                                  ._aspectFlags = convertToImageAspectFlags(descriptor._format),
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