// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/StagingBufferContext.hpp>
#include <VoxFlow/Core/Resources/StagingBuffer.hpp>

namespace VoxFlow
{
constexpr uint64_t STAGING_BUFFER_DEFAULT_SIZE = 1024U * 1024U;

StagingBufferContext::StagingBufferContext(
    LogicalDevice* logicalDevice,
    RenderResourceMemoryPool* renderResourceMemoryPool)
    : _logicalDevice(logicalDevice),
      _renderResourceMemoryPool(renderResourceMemoryPool)
{
}

StagingBufferContext ::~StagingBufferContext()
{
    release();
}

std::tuple<StagingBuffer*, uint64_t>
StagingBufferContext::getOrCreateStagingBuffer(const uint64_t size)
{
    StagingBuffer* stagingBuffer = nullptr;
    uint64_t stagingBufferOffset = UINT32_MAX;

    for (StagingBufferPool& bufferPool : _stagingBufferPools)
    {
        const uint64_t offset = bufferPool._blockAllocator->allocate(size);
        if (offset != BlockAllocator::INVALID_BLOCK_OFFSET)
        {
            stagingBuffer = bufferPool._stagingBuffer.get();
            stagingBufferOffset = offset;
        }
    }

    if (stagingBuffer == nullptr)
    {
        auto stagingBufferPtr = std::make_shared<StagingBuffer>(
            "StagingBuffer", _logicalDevice, _renderResourceMemoryPool);

        stagingBufferPtr->makeAllocationResident(STAGING_BUFFER_DEFAULT_SIZE);

        stagingBuffer = stagingBufferPtr.get();

        _stagingBufferPools.push_back(
            StagingBufferPool{ std::move(stagingBufferPtr),
                               std::make_unique<LinearBlockAllocator>(
                                   STAGING_BUFFER_DEFAULT_SIZE, false) });

        stagingBufferOffset =
            _stagingBufferPools.back()._blockAllocator->allocate(size);
    }

    return { stagingBuffer, stagingBufferOffset };
}

void StagingBufferContext::release()
{
    _stagingBufferPools.clear();
}
}  // namespace VoxFlow