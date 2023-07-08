// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/DeviceQueryContext.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandPool.hpp>
#include <VoxFlow/Core/Devices/Queue.hpp>
#include "../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Queue Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);
    VoxFlow::LogicalDevice logicalDevice(gVulkanContext, &physicalDevice,
                                         &instance);

    VoxFlow::DeviceQueryContext queryContext(&logicalDevice);
    CHECK_EQ(
        queryContext.initialize(VoxFlow::DeviceQueryMode::PerformanceCounter),
        true);

    VoxFlow::Queue* mainQueue = logicalDevice.getQueuePtr("GCT");
    VoxFlow::CommandPool commandPool(&logicalDevice, mainQueue);
    std::shared_ptr<VoxFlow::CommandBuffer> commandBuffer = commandPool.getOrCreateCommandBuffer();
    CHECK_EQ(VoxFlow::DebugUtil::NumValidationErrorDetected, 0);
}