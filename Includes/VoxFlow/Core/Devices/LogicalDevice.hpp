// Author : snowapril

#ifndef VOXEL_FLOW_LOGICAL_DEVICE_HPP
#define VOXEL_FLOW_LOGICAL_DEVICE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/Queue.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>
#include <unordered_map>

namespace VoxFlow
{
class LogicalDevice : NonCopyable
{
 public:
    LogicalDevice(const Context& ctx, const PhysicalDevice& physicalDevice);
    ~LogicalDevice() override;
    LogicalDevice(LogicalDevice&& other) noexcept;
    LogicalDevice& operator=(LogicalDevice&& other) noexcept;

    [[nodiscard]] VkDevice get() const noexcept
    {
        return _device;
    }
    [[nodiscard]] std::weak_ptr<Queue> getQueuePtr(
        const std::string& queueName);

 private:
    void release();

 private:
    VkDevice _device{ VK_NULL_HANDLE };
    std::unordered_map<std::string, std::shared_ptr<Queue>> _queueMap{};
};
}  // namespace VoxFlow

#endif