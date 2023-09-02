// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_RESOURCE_ALLOCATOR_HPP
#define VOXEL_FLOW_RENDER_RESOURCE_ALLOCATOR_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <string>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class Texture;
class Buffer;

class RenderResourceAllocator : private NonCopyable
{
 public:
    explicit RenderResourceAllocator(
        LogicalDevice* logicalDevice,
        RenderResourceMemoryPool* renderResourceMemoryPool);
    ~RenderResourceAllocator();

 public:
    std::shared_ptr<Texture> allocateTexture(const TextureInfo& textureInfo,
                                             std::string&& debugName);

    std::shared_ptr<Buffer> allocateBuffer(const BufferInfo& bufferInfo,
                                           std::string&& debugName);

 protected:
 private:
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool = nullptr;
};
}  // namespace VoxFlow

#endif