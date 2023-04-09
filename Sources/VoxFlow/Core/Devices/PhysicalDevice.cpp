// Author : snowapril

#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
PhysicalDevice::PhysicalDevice(Instance* instance)
{
    uint32_t numPhysicalDevices;
    vkEnumeratePhysicalDevices(instance->get(), &numPhysicalDevices, nullptr);
    std::vector<VkPhysicalDevice> deviceCandidates(numPhysicalDevices);
    vkEnumeratePhysicalDevices(instance->get(), &numPhysicalDevices,
                               deviceCandidates.data());

    // TODO(snowapril) : enable physical device selection between external gpu
    // and internal gpu
    _physicalDevice = deviceCandidates[0];
}

std::vector<VkLayerProperties> PhysicalDevice::getPossibleLayers() const
{
    uint32_t layerCount;
    vkEnumerateDeviceLayerProperties(_physicalDevice, &layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateDeviceLayerProperties(_physicalDevice, &layerCount,
                                     layerProperties.data());
    return layerProperties;
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