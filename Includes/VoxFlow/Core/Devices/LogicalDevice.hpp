// Author : snowapril

#ifndef VOXEL_FLOW_LOGICAL_DEVICE_HPP
#define VOXEL_FLOW_LOGICAL_DEVICE_HPP

#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>

namespace VoxFlow
{
class LogicalDevice : NonCopyable
{
 public:
    LogicalDevice(const Context& ctx, const PhysicalDevice& physicalDevice);
    ~LogicalDevice() override;

 protected:
    void release() const;

 private:
    VkDevice _device{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif