// Author : snowapril

#include <VoxFlow/Core/Resources/RenderResourceAllocator.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
RenderResourceAllocator::RenderResourceAllocator(Instance* instance,
                                                 PhysicalDevice* physicalDevice,
                                                 LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
    _renderResourceMemoryPool = std::make_unique<RenderResourceMemoryPool>(
        logicalDevice, physicalDevice, instance);
      }
RenderResourceAllocator ::~RenderResourceAllocator()
{
}

std::shared_ptr<Texture> RenderResourceAllocator::allocateTexture(
    const TextureInfo& textureInfo, std::string&& debugName)
{
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(
        std::move(debugName), _logicalDevice, _renderResourceMemoryPool.get());

    if (texture->makeResourceResident(textureInfo) == false)
    {
        return nullptr;
    }

    return texture;
}

std::shared_ptr<Buffer> RenderResourceAllocator::allocateBuffer(
    const BufferInfo& bufferInfo, std::string&& debugName)
{
    std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(
        std::move(debugName), _logicalDevice, _renderResourceMemoryPool.get());

    if (buffer->makeResourceResident(bufferInfo) == false)
    {
        return nullptr;
    }

    return texture;
}

}  // namespace VoxFlow