// Author : snowapril

#ifndef VOXEL_FLOW_PHYSICAL_DEVICE_HPP
#define VOXEL_FLOW_PHYSICAL_DEVICE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <vector>

namespace VoxFlow
{
class Instance;

class PhysicalDevice : NonCopyable
{
 public:
    explicit PhysicalDevice(Instance* instance);
    ~PhysicalDevice() override = default;

    [[nodiscard]] std::vector<VkLayerProperties> getPossibleLayers() const;
    [[nodiscard]] std::vector<VkExtensionProperties> getPossibleExtensions()
        const;
    [[nodiscard]] VkPhysicalDeviceMemoryProperties getMemoryProperty() const;
    [[nodiscard]] std::vector<VkQueueFamilyProperties>
    getQueueFamilyProperties() const;

    [[nodiscard]] VkPhysicalDevice get() const noexcept
    {
        return _physicalDevice;
    }

 private:
    VkPhysicalDevice _physicalDevice{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif