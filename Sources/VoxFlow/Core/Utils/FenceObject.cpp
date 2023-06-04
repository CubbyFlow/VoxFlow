// Author : snowapril

#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>

namespace VoxFlow
{
bool FenceObject::isCompleted() const
{
    uint64_t currentSemaphoreValue = _queue->querySemaphoreValue();
    return _timelineValue <= currentSemaphoreValue;
}
bool FenceObject::isCompletedCached() const
{
    return _timelineValue <= _queue->getLastCompletedFenceValue();
}
}  // namespace VoxFlow