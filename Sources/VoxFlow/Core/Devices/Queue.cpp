// Author : snowapril

#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
Queue::Queue(VkQueue queueHandle, uint32_t familyIndex,
             uint32_t queueIndex) noexcept
    : _queue(queueHandle), _familyIndex(familyIndex), _queueIndex(queueIndex)
{
    // DO NOTHING
}

Queue::Queue(Queue&& other) noexcept
    : _queue(std::move(other._queue)),
      _familyIndex(other._familyIndex),
      _queueIndex(other._queueIndex)
{
}

Queue& Queue::operator=(Queue&& other) noexcept
{
    if (this != &other)
    {
        _queue = std::move(other._queue);
        _familyIndex = other._familyIndex;
        _queueIndex = other._queueIndex;
    }
    return *this;
}
}  // namespace VoxFlow