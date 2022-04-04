// Author : snowapril

#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include "VoxFlow/Core/Utils/DecisionMaker.hpp"

namespace VoxFlow
{
PhysicalDevice::PhysicalDevice(const Instance& instance)
{
    uint32_t numPhysicalDevices;
    vkEnumeratePhysicalDevices(instance.get(), &numPhysicalDevices, nullptr);
    std::vector<VkPhysicalDevice> deviceCandidates;
    vkEnumeratePhysicalDevices(instance.get(), &numPhysicalDevices,
                               deviceCandidates.data());

    _physicalDevice = deviceCandidates[0];
}

std::vector<VkExtensionProperties> PhysicalDevice::getPossibleExtensions() const
{
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(_physicalDevice, nullptr,
                                         &extensionCount, nullptr);
    std::vector<VkExtensionProperties> extensionProperties(extensionCount);
    vkEnumerateDeviceExtensionProperties(
        _physicalDevice, nullptr, &extensionCount, extensionProperties.data());
    return extensionProperties;
}

VkPhysicalDeviceMemoryProperties PhysicalDevice::getMemoryProperty() const
{
    VkPhysicalDeviceMemoryProperties memoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physicalDevice, &memoryProperties);
    return memoryProperties;
}

std::vector<VkQueueFamilyProperties> PhysicalDevice::getQueueFamilyProperties()
    const
{
    uint32_t familyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &familyCount,
                                             nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(familyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(_physicalDevice, &familyCount,
                                             queueFamilyProperties.data());
    return queueFamilyProperties;
}

}  // namespace VoxFlow