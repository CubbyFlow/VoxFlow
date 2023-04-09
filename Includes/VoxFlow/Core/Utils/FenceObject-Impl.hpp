// Author : snowapril

#ifndef VOXEL_FLOW_FENCE_OBJECT_IMPL_HPP
#define VOXEL_FLOW_FENCE_OBJECT_IMPL_HPP

#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>

namespace VoxFlow
{
class Queue;

inline bool FenceObject::isCompleted() const
{
    uint64_t currentSemaphoreValue = _queue->getTimelineSemaphoreValue();
    return _timelineValue <= currentSemaphoreValue;
}
inline bool FenceObject::isCompletedCached() const
{
    return _timelineValue <= _queue->getLastCompletedFenceValue();
}
}  // namespace VoxFlow

#endif