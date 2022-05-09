// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Compute Pipeline Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, physicalDevice);

    auto pipelineLayout =
        std::make_shared<VoxFlow::PipelineLayout>(logicalDevice);

    VoxFlow::ComputePipeline testPipeline(
        logicalDevice, { RESOURCES_DIR "/Shaders/test_shader.comp" },
        pipelineLayout);

    CHECK_NE(testPipeline.get(), VK_NULL_HANDLE);
}