// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/StagingBufferContext.hpp>
#include <VoxFlow/Core/Resources/StagingBuffer.hpp>

namespace VoxFlow
{
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

StagingBuffer* StagingBufferContext::getOrCreateStagingBuffer(const uint32_t size)
{
    return nullptr;
}

void StagingBufferContext::release()
{
}
}  // namespace VoxFlow