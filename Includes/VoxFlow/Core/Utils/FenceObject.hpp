// Author : snowapril

#ifndef VOXEL_FLOW_FENCE_OBJECT_HPP
#define VOXEL_FLOW_FENCE_OBJECT_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class Queue;

class FenceObject
{
 public:
    FenceObject() = default;
    explicit FenceObject(Queue* queue, const uint64_t initialTimelineValue = UINT64_MAX)
        : _queue(queue), _timelineValue(initialTimelineValue)
    {
    }
    virtual ~FenceObject() = default;
    FenceObject(const FenceObject&) = default;
    FenceObject(FenceObject&&) = default;
    FenceObject& operator=(const FenceObject&) = default;
    FenceObject& operator=(FenceObject&&) = default;

 public:
    inline Queue* getQueue()
    {
        return _queue;
    }
    inline bool isCompleted() const
    {
        return true;  // TODO(snowapril) : 
    }
    inline bool isCompletedCached() const
    {
        return true; // TODO(snowapril) : 
    }
    inline bool isValid() const
    {
        return (_queue != nullptr) && (_timelineValue != UINT64_MAX);
    }
    inline uint64_t getFenceValue() const
    {
        return _timelineValue;
    }
    inline uint64_t advanceFenceValue() 
    {
        return (++_timelineValue);
    }

 private:
    Queue* _queue = nullptr;
    uint64_t _timelineValue = UINT64_MAX;
};
}  // namespace VoxFlow

#endif