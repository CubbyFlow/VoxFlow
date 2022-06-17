// Author : snowapril

#ifndef VOXEL_FLOW_QUEUE_HPP
#define VOXEL_FLOW_QUEUE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class Queue : NonCopyable
{
 public:
    explicit Queue(VkQueue queueHandle, unsigned int familyIndex) noexcept;
    ~Queue() override = default;
    Queue(Queue&& other) noexcept;
    Queue& operator=(Queue&& other) noexcept;

    [[nodiscard]] VkQueue get() const noexcept
    {
        return _queue;
    }
    [[nodiscard]] unsigned int getFamilyIndex() const noexcept
    {
        return _familyIndex;
    }

 private:
    VkQueue _queue{ VK_NULL_HANDLE };
    unsigned int _familyIndex{ 0 };
};
}  // namespace VoxFlow

#endif