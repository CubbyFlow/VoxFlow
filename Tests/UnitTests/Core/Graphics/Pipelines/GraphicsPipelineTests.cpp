// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetLayout.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Graphics Pipeline Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, &physicalDevice, &instance);

    const VoxFlow::RenderPass renderPass(logicalDevice.get());

    std::vector<std::shared_ptr<VoxFlow::ShaderModule>> shaderModules;
    shaderModules.emplace_back(std::make_shared<VoxFlow::ShaderModule>(
        logicalDevice.get(), RESOURCES_DIR "/Shaders/test_shader.vert"));
    shaderModules.emplace_back(std::make_shared<VoxFlow::ShaderModule>(
        logicalDevice.get(), RESOURCES_DIR "/Shaders/test_shader.frag"));

    std::vector<std::shared_ptr<VoxFlow::DescriptorSetLayout>> setLayouts;
    setLayouts.push_back(shaderModules[0]->getDescriptorSetLayout());
    setLayouts.push_back(shaderModules[1]->getDescriptorSetLayout());

    const auto pipelineLayout =
        std::make_shared<VoxFlow::PipelineLayout>(logicalDevice.get(), setLayouts);

    const VoxFlow::GraphicsPipeline testPipeline(
        logicalDevice.get(), std::move(shaderModules), pipelineLayout);

    CHECK_NE(testPipeline.get(), VK_NULL_HANDLE);
}