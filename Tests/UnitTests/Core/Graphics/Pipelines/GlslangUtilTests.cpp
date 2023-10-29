// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderUtil.hpp>
#include <array>
#include <sstream>
#include "../../../UnitTestUtils.hpp"

TEST_CASE("Glslang Stage parsing from filename")
{
    CHECK_EQ(VoxFlow::GlslangUtil::GlslangStageFromFilename("test_shader.vert"),
             GLSLANG_STAGE_VERTEX);
    CHECK_EQ(VoxFlow::GlslangUtil::GlslangStageFromFilename("test_shader.frag"),
             GLSLANG_STAGE_FRAGMENT);
    CHECK_EQ(VoxFlow::GlslangUtil::GlslangStageFromFilename("test_shader.comp"),
             GLSLANG_STAGE_COMPUTE);
    CHECK_EQ(VoxFlow::GlslangUtil::GlslangStageFromFilename("test_shader.geom"),
             GLSLANG_STAGE_GEOMETRY);
    CHECK_EQ(VoxFlow::GlslangUtil::GlslangStageFromFilename("test_shader.tesc"),
             GLSLANG_STAGE_TESSCONTROL);
    CHECK_EQ(VoxFlow::GlslangUtil::GlslangStageFromFilename("test_shader.tese"),
             GLSLANG_STAGE_TESSEVALUATION);
}

TEST_CASE("Glslang Stage to vulkan stage")
{
    CHECK_EQ(
        VoxFlow::GlslangUtil::GlslangStageToVulkanStage(GLSLANG_STAGE_VERTEX),
        VK_SHADER_STAGE_VERTEX_BIT);
    CHECK_EQ(
        VoxFlow::GlslangUtil::GlslangStageToVulkanStage(GLSLANG_STAGE_FRAGMENT),
        VK_SHADER_STAGE_FRAGMENT_BIT);
    CHECK_EQ(
        VoxFlow::GlslangUtil::GlslangStageToVulkanStage(GLSLANG_STAGE_COMPUTE),
        VK_SHADER_STAGE_COMPUTE_BIT);
    CHECK_EQ(
        VoxFlow::GlslangUtil::GlslangStageToVulkanStage(GLSLANG_STAGE_GEOMETRY),
        VK_SHADER_STAGE_GEOMETRY_BIT);
    CHECK_EQ(VoxFlow::GlslangUtil::GlslangStageToVulkanStage(
                 GLSLANG_STAGE_TESSCONTROL),
             VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT);
    CHECK_EQ(VoxFlow::GlslangUtil::GlslangStageToVulkanStage(
                 GLSLANG_STAGE_TESSEVALUATION),
             VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT);
}

TEST_CASE("Glslang Shader file reading")
{
    std::vector<char> source;
    CHECK(VoxFlow::GlslangUtil::ReadShaderFile(
        RESOURCES_DIR "/Shaders/test_shader.vert", &source));

    std::string firstWord;
    std::istringstream isstr(source.data());
    isstr >> firstWord;
    CHECK_EQ(firstWord, "#version");
}

TEST_CASE("Glslang Test Shader Compile")
{
    // Vulkan device setup for glslang_initialize_process
    VoxFlow::Instance instance(gVulkanContext);
    VoxFlow::PhysicalDevice physicalDevice(&instance);
    const auto logicalDevice = std::make_shared<VoxFlow::LogicalDevice>(
        gVulkanContext, &physicalDevice, &instance,
        VoxFlow::LogicalDeviceType::MainDevice);
    (void)logicalDevice;

    constexpr std::array<std::pair<glslang_stage_t, const char*>, 3>
        targetShaders = { std::make_pair(GLSLANG_STAGE_VERTEX, RESOURCES_DIR
                                         "/Shaders/test_shader.vert"),
                          std::make_pair(GLSLANG_STAGE_FRAGMENT, RESOURCES_DIR
                                         "/Shaders/test_shader.frag"),
                          std::make_pair(GLSLANG_STAGE_COMPUTE, RESOURCES_DIR
                                         "/Shaders/test_shader.comp") };

    for (const auto& shader : targetShaders)
    {
        std::vector<char> source;
        CHECK(VoxFlow::GlslangUtil::ReadShaderFile(shader.second, &source));

        std::vector<unsigned int> spirvBinary;
        CHECK(VoxFlow::GlslangUtil::CompileShader(shader.first, source.data(),
                                                  &spirvBinary));
        CHECK(spirvBinary.size() > 0);
    }
}