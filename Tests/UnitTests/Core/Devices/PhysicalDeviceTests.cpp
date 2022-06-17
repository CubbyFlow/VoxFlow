// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include "../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Physical Device Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);

    VoxFlow::PhysicalDevice physicalDevice(instance);
    CHECK_NE(physicalDevice.get(), VK_NULL_HANDLE);
}

TEST_CASE("Vulkan Physical Device Feature Querying")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(instance);

    CHECK(physicalDevice.getMemoryProperty().memoryTypeCount != 0);
    CHECK(physicalDevice.getMemoryProperty().memoryHeapCount != 0);
    CHECK(physicalDevice.getPossibleExtensions().empty() == false);
    CHECK(physicalDevice.getQueueFamilyProperties().empty() == false);
}