// Author : snowapril

#ifndef VOXEL_FLOW_STAGING_BUFFER_MANAGER_HPP
#define VOXEL_FLOW_STAGING_BUFFER_MANAGER_HPP

#include <VoxFlow/Core/Utils/MemoryAllocator.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;

class StagingBufferManager : private NonCopyable
{
 public:
    explicit StagingBufferManager(
        LogicalDevice* logicalDevice,
        RenderResourceMemoryPool* renderResourceMemoryPool);
    ~StagingBufferManager();

 public:
    Buffer* getOrCreateStagingBuffer(const uint32_t size);

    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool;
    LinearBlockAllocator _blockAllocator;
    Buffer* _stagingBuffer = nullptr;
};
}  // namespace VoxFlow

#endif