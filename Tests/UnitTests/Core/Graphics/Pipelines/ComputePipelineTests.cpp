// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Compute Pipeline Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, &physicalDevice, &instance,
        VoxFlow::LogicalDeviceType::MainDevice);

    VoxFlow::ComputePipeline testPipeline(
        logicalDevice.get(), RESOURCES_DIR "/Shaders/test_shader.comp");

    const bool result = testPipeline.initialize();
    CHECK_EQ(result, true);
    CHECK_NE(testPipeline.get(), VK_NULL_HANDLE);
    CHECK_EQ(VoxFlow::DebugUtil::NumValidationErrorDetected, 0);
}