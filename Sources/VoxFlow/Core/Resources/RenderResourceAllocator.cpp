// Author : snowapril

#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/RenderResourceAllocator.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
RenderResourceAllocator::RenderResourceAllocator(LogicalDevice* logicalDevice, RenderResourceMemoryPool* renderResourceMemoryPool)
    : _logicalDevice(logicalDevice), _renderResourceMemoryPool(renderResourceMemoryPool)
{
}

RenderResourceAllocator::~RenderResourceAllocator()
{
    if (_renderResourceMemoryPool != nullptr)
    {
        delete _renderResourceMemoryPool;
    }
}

std::shared_ptr<Texture> RenderResourceAllocator::allocateTexture(const TextureInfo& textureInfo, std::string&& debugName)
{
    std::shared_ptr<Texture> texture = std::make_shared<Texture>(std::move(debugName), _logicalDevice, _renderResourceMemoryPool);

    if (texture->makeAllocationResident(textureInfo) == false)
    {
        return nullptr;
    }

    return texture;
}

std::shared_ptr<Buffer> RenderResourceAllocator::allocateBuffer(const BufferInfo& bufferInfo, std::string&& debugName)
{
    std::shared_ptr<Buffer> buffer = std::make_shared<Buffer>(std::move(debugName), _logicalDevice, _renderResourceMemoryPool);

    if (buffer->makeAllocationResident(bufferInfo) == false)
    {
        return nullptr;
    }

    return buffer;
}

}  // namespace VoxFlow