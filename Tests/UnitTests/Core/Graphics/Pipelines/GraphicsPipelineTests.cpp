// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Graphics Pipeline Initialization")
{
    const VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, physicalDevice);

    const auto pipelineLayout =
        std::make_shared<VoxFlow::PipelineLayout>(logicalDevice);

    const VoxFlow::GraphicsPipeline testPipeline(
        logicalDevice,
        { RESOURCES_DIR "/Shaders/test_shader.vert",
          RESOURCES_DIR "/Shaders/test_shader.frag" },
        VoxFlow::PipelineCreateInfo::CreateDefault(), pipelineLayout);

    CHECK_NE(testPipeline.get(), VK_NULL_HANDLE);
}