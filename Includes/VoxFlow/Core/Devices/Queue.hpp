// Author : snowapril

#ifndef VOXEL_FLOW_QUEUE_HPP
#define VOXEL_FLOW_QUEUE_HPP

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>

namespace VoxFlow
{
class Queue : NonCopyable
{
 public:
    explicit Queue(const LogicalDevice& device, uint32_t familyIndex,
                   uint32_t queueIndex = 0);
    ~Queue() override = default;
    Queue(Queue&& queue);
    Queue& operator=(Queue&& queue);

    [[nodiscard]] VkQueue get() const
    {
        return _queue;
    }
    [[nodiscard]] uint32_t getFamilyIndex() const
    {
        return _familyIndex;
    }

 private:
    VkQueue _queue{ VK_NULL_HANDLE };
    uint32_t _familyIndex{ 0 };
};
}  // namespace VoxFlow

#endif