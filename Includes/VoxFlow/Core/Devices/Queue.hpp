// Author : snowapril

#ifndef VOXEL_FLOW_QUEUE_HPP
#define VOXEL_FLOW_QUEUE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;

class Queue : NonCopyable
{
 public:
    explicit Queue(VkQueue queueHandle, uint32_t familyIndex,
                   uint32_t queueIndex) noexcept;
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
    uint32_t _familyIndex{ 0 };
    uint32_t _queueIndex{ 0 };
    VkQueue _queue{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif