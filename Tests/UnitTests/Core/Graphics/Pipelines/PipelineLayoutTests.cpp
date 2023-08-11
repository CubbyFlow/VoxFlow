// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Pipeline Layout Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);

    auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, &physicalDevice, &instance,
        VoxFlow::LogicalDeviceType::MainDevice);

    const std::vector<const char*> shaderFilePaths{
        RESOURCES_DIR "/Shaders/test_shader.vert",
        RESOURCES_DIR "/Shaders/test_shader.frag"
    };

    std::vector<std::unique_ptr<VoxFlow::ShaderModule>> shaderModules;
    std::vector<const VoxFlow::ShaderReflectionDataGroup*> reflectionDataGroups;
    for (const char* shaderPath : shaderFilePaths)
    {
        shaderModules.push_back(std::make_unique<VoxFlow::ShaderModule>(
            logicalDevice.get(), shaderPath));

        reflectionDataGroups.push_back(
            shaderModules.back()->getShaderReflectionDataGroup());
    }

    const auto pipelineLayout =
        std::make_shared<VoxFlow::PipelineLayout>(logicalDevice.get());

    CHECK_EQ(pipelineLayout->initialize(std::move(reflectionDataGroups)), true);
    CHECK_NE(pipelineLayout->get(), VK_NULL_HANDLE);
    CHECK_EQ(VoxFlow::DebugUtil::NumValidationErrorDetected, 0);
}