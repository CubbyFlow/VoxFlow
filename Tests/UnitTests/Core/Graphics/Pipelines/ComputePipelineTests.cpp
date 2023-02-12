// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Compute Pipeline Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, physicalDevice);

    auto shaderModule = std::make_shared<VoxFlow::ShaderModule>(
        logicalDevice.get(), RESOURCES_DIR "/Shaders/test_shader.comp");

    std::vector<std::shared_ptr<VoxFlow::DescriptorSetLayout>> setLayouts;
    setLayouts.push_back(shaderModule->getDescriptorSetLayout());

    auto pipelineLayout = std::make_shared<VoxFlow::PipelineLayout>(
        logicalDevice.get(), setLayouts);


    VoxFlow::ComputePipeline testPipeline(logicalDevice.get(), shaderModule,
                                          pipelineLayout);

    CHECK_NE(testPipeline.get(), VK_NULL_HANDLE);
}