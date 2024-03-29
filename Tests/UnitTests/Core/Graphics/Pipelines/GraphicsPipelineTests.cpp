// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStreamingContext.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Graphics Pipeline Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, &physicalDevice, &instance,
        VoxFlow::LogicalDeviceType::MainDevice);

    std::shared_ptr<VoxFlow::RenderPass> renderPass =
        std::make_shared<VoxFlow::RenderPass>(logicalDevice.get());

    VoxFlow::RenderTargetLayoutKey dummyRTKey = {
        ._debugName = "DummyRenderPass",
        ._colorFormats = { VK_FORMAT_R8G8B8A8_UNORM },
        ._depthStencilFormat = std::nullopt,
    };

    renderPass->initialize(dummyRTKey);

    std::shared_ptr<VoxFlow::GraphicsPipeline> testPipeline =
        logicalDevice->getPipelineStreamingContext()->createGraphicsPipeline(
            { "test_shader.vert", "test_shader.frag" });

    const bool result = testPipeline->initialize(renderPass.get());
    CHECK_EQ(result, true);
    CHECK_NE(testPipeline.get(), VK_NULL_HANDLE);
    CHECK_EQ(VoxFlow::DebugUtil::NumValidationErrorDetected, 0);
}