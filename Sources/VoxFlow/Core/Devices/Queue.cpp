// Author : snowapril

#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
Queue::Queue(const LogicalDevice& device, uint32_t familyIndex,
             uint32_t queueIndex /* 0 */)
    : _familyIndex(familyIndex)
{
    vkGetDeviceQueue(device.get(), familyIndex, queueIndex, &_queue);
}

Queue::Queue(Queue&& other) noexcept
    : _queue(std::move(other._queue)), _familyIndex(other._familyIndex)
{
    // Do nothing
}

Queue& Queue::operator=(Queue&& other) noexcept
{
    if (this != &other)
    {
        _queue = std::move(other._queue);
        _familyIndex = other._familyIndex;
    }
    return *this;
}
}  // namespace VoxFlow