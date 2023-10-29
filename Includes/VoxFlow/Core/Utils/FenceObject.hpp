// Author : snowapril

#ifndef VOXEL_FLOW_FENCE_OBJECT_HPP
#define VOXEL_FLOW_FENCE_OBJECT_HPP

#include <cstdint>

namespace VoxFlow
{
class Queue;

class FenceObject
{
 public:
    explicit FenceObject(Queue* queue, const uint64_t initialTimelineValue) : _queue(queue), _timelineValue(initialTimelineValue)
    {
    }
    ~FenceObject() = default;
    FenceObject(const FenceObject&) = default;
    FenceObject(FenceObject&&) = default;
    FenceObject& operator=(const FenceObject&) = default;
    FenceObject& operator=(FenceObject&&) = default;

    static inline FenceObject Default()
    {
        return FenceObject(nullptr, UINT64_MAX);
    }

 public:
    // Get fence dedicated queue instance.
    inline Queue* getQueue() const
    {
        return _queue;
    }

    // Returns whether this fence instance is valid or not
    inline bool isValid() const
    {
        return (_queue != nullptr) && (_timelineValue != UINT64_MAX);
    }

    // Returns the expected timeline semaphore value of the queue
    inline uint64_t getFenceValue() const
    {
        return _timelineValue;
    }

    // Advance fence value
    inline uint64_t advanceFenceValue()
    {
        return (++_timelineValue);
    }

    // Returns whether queue's current fence value reach to expected one (synced)
    bool isCompleted() const;

    // Returns whether queue's current fence value reach to expected one.
    // This could returns false even current fence value of queue reached to expected one.
    bool isCompletedCached() const;

 private:
    Queue* _queue = nullptr;
    uint64_t _timelineValue = UINT64_MAX;
};
}  // namespace VoxFlow

#endif