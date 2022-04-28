// Author : snowapril

#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
Queue::Queue(VkQueue queueHandle, uint32_t familyIndex) noexcept
    : _queue(queueHandle), _familyIndex(familyIndex)
{
    // Do nothing
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