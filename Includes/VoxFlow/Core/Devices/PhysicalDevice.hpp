// Author : snowapril

#ifndef VOXEL_FLOW_PHYSICAL_DEVICE_HPP
#define VOXEL_FLOW_PHYSICAL_DEVICE_HPP

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>

namespace VoxFlow
{
class PhysicalDevice : NonCopyable
{
 public:
    PhysicalDevice(const Instance& instance);
    ~PhysicalDevice() override = default;

    [[nodiscard]] std::vector<VkExtensionProperties> getPossibleExtensions()
        const;
    [[nodiscard]] VkPhysicalDeviceMemoryProperties getMemoryProperty() const;
    [[nodiscard]] std::vector<VkQueueFamilyProperties>
    getQueueFamilyProperties() const;

 private:
    VkPhysicalDevice _physicalDevice{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif