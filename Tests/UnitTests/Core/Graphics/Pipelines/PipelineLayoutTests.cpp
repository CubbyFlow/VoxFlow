// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Pipeline Layout Initialization")
{
    const VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, physicalDevice);

    const auto pipelineLayout =
        std::make_shared<VoxFlow::PipelineLayout>(logicalDevice);

    CHECK_NE(pipelineLayout->get(), VK_NULL_HANDLE);
}