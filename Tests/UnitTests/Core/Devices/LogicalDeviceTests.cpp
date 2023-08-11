// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include "../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Logical Device Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);
    VoxFlow::LogicalDevice logicalDevice(gVulkanContext, &physicalDevice, &instance,
        VoxFlow::LogicalDeviceType::MainDevice);
    CHECK_NE(logicalDevice.get(), VK_NULL_HANDLE);
    CHECK_EQ(VoxFlow::DebugUtil::NumValidationErrorDetected, 0);
}

TEST_CASE("Vulkan Logical Device Queue Creation")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);
    VoxFlow::LogicalDevice logicalDevice(gVulkanContext, &physicalDevice, &instance,
        VoxFlow::LogicalDeviceType::MainDevice);
    CHECK_EQ(VoxFlow::DebugUtil::NumValidationErrorDetected, 0);
}