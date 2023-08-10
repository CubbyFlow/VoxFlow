// Author : snowapril

#ifndef VOXEL_FLOW_STAGING_BUFFER_MANAGER_HPP
#define VOXEL_FLOW_STAGING_BUFFER_MANAGER_HPP

#include <VoxFlow/Core/Utils/MemoryAllocator.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <optional>
#include <tuple>

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
    std::tuple<StagingBuffer*, uint32_t> getOrCreateStagingBuffer(
        const uint32_t size);

    void release();

 private:
    struct StagingBufferPool
    {
        std::shared_ptr<StagingBuffer> _stagingBuffer;
        std::unique_ptr<LinearBlockAllocator> _blockAllocator;
    };

    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool;
    std::vector<StagingBufferPool> _stagingBufferPools;
};
}  // namespace VoxFlow

#endif