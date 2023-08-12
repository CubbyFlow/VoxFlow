// Author : snowapril

#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/RenderResourceAllocator.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
RenderResourceAllocator::RenderResourceAllocator(Instance* instance,
                                                 PhysicalDevice* physicalDevice,
                                                 LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
    _renderResourceMemoryPool =
        new RenderResourceMemoryPool(logicalDevice, physicalDevice, instance);
}

RenderResourceAllocator ::~RenderResourceAllocator()
{
    if (_renderResourceMemoryPool != nullptr)
    {
        delete _renderResourceMemoryPool;
    }
}

Handle<Texture> RenderResourceAllocator::allocateTexture(
    const TextureInfo& textureInfo, std::string&& debugName)
{
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(
        std::move(debugName), _logicalDevice, _renderResourceMemoryPool);

    if (texture->makeAllocationResident(textureInfo) == false)
    {
        return nullptr;
    }

    return texture;
}

Handle<Buffer> RenderResourceAllocator::allocateBuffer(
    const BufferInfo& bufferInfo, std::string&& debugName)
{
    std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(
        std::move(debugName), _logicalDevice, _renderResourceMemoryPool);

    if (buffer->makeAllocationResident(bufferInfo) == false)
    {
        return nullptr;
    }

    return buffer;
}

}  // namespace VoxFlow