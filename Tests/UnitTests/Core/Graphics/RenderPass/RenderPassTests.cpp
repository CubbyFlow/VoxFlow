// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan RenderPass Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, &physicalDevice, &instance,
        VoxFlow::LogicalDeviceType::MainDevice);

    const auto renderPass =
        std::make_shared<VoxFlow::RenderPass>(logicalDevice.get());

    VoxFlow::RenderTargetLayoutKey dummyRTKey = {
        ._debugName = "DummyRenderPass",
        ._colorFormats = { VK_FORMAT_R8G8B8A8_UNORM },
        ._depthStencilFormat = std::nullopt,
    };

    CHECK_EQ(renderPass->initialize(dummyRTKey), true);
    CHECK_NE(renderPass->get(), VK_NULL_HANDLE);
    CHECK_EQ(VoxFlow::DebugUtil::NumValidationErrorDetected, 0);
}