// Author : snowapril

#ifndef VOXEL_FLOW_QUEUE_HPP
#define VOXEL_FLOW_QUEUE_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>
#include <cstdint>

namespace VoxFlow
{
class Queue : NonCopyable
{
 public:
    explicit Queue(VkQueue queueHandle, uint32_t familyIndex) noexcept;
    ~Queue() override = default;
    Queue(Queue&& other) noexcept;
    Queue& operator=(Queue&& other) noexcept;

    [[nodiscard]] VkQueue get() const noexcept
    {
        return _queue;
    }
    [[nodiscard]] uint32_t getFamilyIndex() const noexcept
    {
        return _familyIndex;
    }

 private:
    VkQueue     _queue       { VK_NULL_HANDLE };
    uint32_t    _familyIndex { 0 };
};
}  // namespace VoxFlow

#endif