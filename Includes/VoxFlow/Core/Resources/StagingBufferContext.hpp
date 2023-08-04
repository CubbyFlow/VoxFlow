// Author : snowapril

#ifndef VOXEL_FLOW_STAGING_BUFFER_MANAGER_HPP
#define VOXEL_FLOW_STAGING_BUFFER_MANAGER_HPP

#include <VoxFlow/Core/Utils/MemoryAllocator.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class StagingBuffer;

class StagingBufferContext : private NonCopyable
{
 public:
    explicit StagingBufferContext(
        LogicalDevice* logicalDevice,
        RenderResourceMemoryPool* renderResourceMemoryPool);
    ~StagingBufferContext();

 public:
    StagingBuffer* getOrCreateStagingBuffer(const uint32_t size);

    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool;
    LinearBlockAllocator _blockAllocator;
    StagingBuffer* _stagingBuffer = nullptr;
};
}  // namespace VoxFlow

#endif