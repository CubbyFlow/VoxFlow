// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>
#include <VoxFlow/Core/Resources/StagingBufferManager.hpp>

namespace VoxFlow
{
StagingBufferManager::StagingBufferManager(
    LogicalDevice* logicalDevice,
    RenderResourceMemoryPool* renderResourceMemoryPool)
    : _logicalDevice(logicalDevice),
      _renderResourceMemoryPool(renderResourceMemoryPool)
{
}

StagingBufferManager ::~StagingBufferManager()
{
    release();
}

Buffer* StagingBufferManager::getOrCreateStagingBuffer(const uint32_t size)
{
    return nullptr;
}

void StagingBufferManager::release()
{
}
}  // namespace VoxFlow