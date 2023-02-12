// Author : snowapril

#ifndef VOXEL_FLOW_BUFFER_HPP
#define VOXEL_FLOW_BUFFER_HPP

#include <volk/volk.h>
#include <vma/include/vk_mem_alloc.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;

class Buffer : private NonCopyable
{
 public:
    explicit Buffer(std::string&& debugName, LogicalDevice* logicalDevice, RenderResourceMemoryPool* renderResourceMemoryPool);
    ~Buffer();

 public:
    [[nodiscard]] inline VkBuffer get() const
    {
        return _vkBuffer;
    }

    inline BufferInfo getBufferInfo() const
    {
        return _bufferInfo;
    }

    bool initialize(BufferInfo bufferInfo);
    void release();

 protected:
 private:
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool = nullptr;
    VkBuffer _vkBuffer = VK_NULL_HANDLE;
    VmaAllocation _bufferAllocation = nullptr;
    BufferInfo _bufferInfo;
    std::string _debugName;
};
}  // namespace VoxFlow

#endif