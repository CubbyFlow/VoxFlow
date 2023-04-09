// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Graphics Pipeline Initialization")
{
    const VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, physicalDevice);

    const auto pipelineLayout =
        std::make_shared<VoxFlow::PipelineLayout>(logicalDevice);

    const VoxFlow::RenderPass renderPass(logicalDevice);

    const VkViewport viewport = { .x = 0.0f,
                                  .y = 0.0f,
                                  .width = 1.0f,
                                  .height = 1.0f,
                                  .minDepth = 0.0f,
                                  .maxDepth = 1.0f };
    const VkRect2D scissor = { .offset = { .x = 0, .y = 0 },
                               .extent = { .width = 1u, .height = 1u } };

    auto pipelineCreateInfo = VoxFlow::PipelineCreateInfo::CreateDefault();
    pipelineCreateInfo.renderPass = renderPass.get();
    pipelineCreateInfo.viewportState.viewportCount = 1;
    pipelineCreateInfo.viewportState.scissorCount = 1;
    pipelineCreateInfo.viewportState.pViewports = &viewport;
    pipelineCreateInfo.viewportState.pScissors = &scissor;

    const VoxFlow::GraphicsPipeline testPipeline(
        logicalDevice,
        { RESOURCES_DIR "/Shaders/test_shader.vert",
          RESOURCES_DIR "/Shaders/test_shader.frag" },
        pipelineCreateInfo, pipelineLayout);

    CHECK_NE(testPipeline.get(), VK_NULL_HANDLE);
}