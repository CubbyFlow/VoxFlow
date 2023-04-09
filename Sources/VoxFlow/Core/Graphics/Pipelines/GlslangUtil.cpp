// Author : snowapril

#include <ResourceLimits.h>
#include <glslang_c_interface.h>
#include <spdlog/spdlog.h>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <fstream>

namespace VoxFlow
{
// Referenced on https://github.com/KhronosGroup/glslang/issues/2207
constexpr TBuiltInResource GetDefaultGlslangResource()
{
    constexpr TLimits limits = { .nonInductiveForLoops = true,
                                 .whileLoops = true,
                                 .doWhileLoops = true,
                                 .generalUniformIndexing = true,
                                 .generalAttributeMatrixVectorIndexing = true,
                                 .generalVaryingIndexing = true,
                                 .generalSamplerIndexing = true,
                                 .generalVariableIndexing = true,
                                 .generalConstantMatrixVectorIndexing = true };

    return { .maxLights = 32,
             .maxClipPlanes = 6,
             .maxTextureUnits = 32,
             .maxTextureCoords = 32,
             .maxVertexAttribs = 64,
             .maxVertexUniformComponents = 4096,
             .maxVaryingFloats = 64,
             .maxVertexTextureImageUnits = 32,
             .maxCombinedTextureImageUnits = 80,
             .maxTextureImageUnits = 32,
             .maxFragmentUniformComponents = 4096,
             .maxDrawBuffers = 32,
             .maxVertexUniformVectors = 128,
             .maxVaryingVectors = 8,
             .maxFragmentUniformVectors = 16,
             .maxVertexOutputVectors = 16,
             .maxFragmentInputVectors = 15,
             .minProgramTexelOffset = -8,
             .maxProgramTexelOffset = 7,
             .maxClipDistances = 8,
             .maxComputeWorkGroupCountX = 65535,
             .maxComputeWorkGroupCountY = 65535,
             .maxComputeWorkGroupCountZ = 65535,
             .maxComputeWorkGroupSizeX = 1024,
             .maxComputeWorkGroupSizeY = 1024,
             .maxComputeWorkGroupSizeZ = 64,
             .maxComputeUniformComponents = 1024,
             .maxComputeTextureImageUnits = 16,
             .maxComputeImageUniforms = 8,
             .maxComputeAtomicCounters = 8,
             .maxComputeAtomicCounterBuffers = 1,
             .maxVaryingComponents = 60,
             .maxVertexOutputComponents = 64,
             .maxGeometryInputComponents = 64,
             .maxGeometryOutputComponents = 128,
             .maxFragmentInputComponents = 128,
             .maxImageUnits = 8,
             .maxCombinedImageUnitsAndFragmentOutputs = 8,
             .maxCombinedShaderOutputResources = 8,
             .maxImageSamples = 0,
             .maxVertexImageUniforms = 0,
             .maxTessControlImageUniforms = 0,
             .maxTessEvaluationImageUniforms = 0,
             .maxGeometryImageUniforms = 0,
             .maxFragmentImageUniforms = 8,
             .maxCombinedImageUniforms = 8,
             .maxGeometryTextureImageUnits = 16,
             .maxGeometryOutputVertices = 256,
             .maxGeometryTotalOutputComponents = 1024,
             .maxGeometryUniformComponents = 1024,
             .maxGeometryVaryingComponents = 64,
             .maxTessControlInputComponents = 128,
             .maxTessControlOutputComponents = 128,
             .maxTessControlTextureImageUnits = 16,
             .maxTessControlUniformComponents = 1024,
             .maxTessControlTotalOutputComponents = 4096,
             .maxTessEvaluationInputComponents = 128,
             .maxTessEvaluationOutputComponents = 128,
             .maxTessEvaluationTextureImageUnits = 16,
             .maxTessEvaluationUniformComponents = 1024,
             .maxTessPatchComponents = 120,
             .maxPatchVertices = 32,
             .maxTessGenLevel = 64,
             .maxViewports = 16,
             .maxVertexAtomicCounters = 0,
             .maxTessControlAtomicCounters = 0,
             .maxTessEvaluationAtomicCounters = 0,
             .maxGeometryAtomicCounters = 0,
             .maxFragmentAtomicCounters = 8,
             .maxCombinedAtomicCounters = 8,
             .maxAtomicCounterBindings = 1,
             .maxVertexAtomicCounterBuffers = 0,
             .maxTessControlAtomicCounterBuffers = 0,
             .maxTessEvaluationAtomicCounterBuffers = 0,
             .maxGeometryAtomicCounterBuffers = 0,
             .maxFragmentAtomicCounterBuffers = 1,
             .maxCombinedAtomicCounterBuffers = 1,
             .maxAtomicCounterBufferSize = 16384,
             .maxTransformFeedbackBuffers = 4,
             .maxTransformFeedbackInterleavedComponents = 64,
             .maxCullDistances = 8,
             .maxCombinedClipAndCullDistances = 8,
             .maxSamples = 4,
             .maxMeshOutputVerticesNV = 256,
             .maxMeshOutputPrimitivesNV = 512,
             .maxMeshWorkGroupSizeX_NV = 32,
             .maxMeshWorkGroupSizeY_NV = 1,
             .maxMeshWorkGroupSizeZ_NV = 1,
             .maxTaskWorkGroupSizeX_NV = 32,
             .maxTaskWorkGroupSizeY_NV = 1,
             .maxTaskWorkGroupSizeZ_NV = 1,
             .maxMeshViewCountNV = 4,
             .limits = limits };
}

glslang_stage_t GlslangUtil::GlslangStageFromFilename(
    const std::string_view filename)
{
    if (filename.find(".vert") != std::string_view::npos)
        return GLSLANG_STAGE_VERTEX;
    if (filename.find(".frag") != std::string_view::npos)
        return GLSLANG_STAGE_FRAGMENT;
    if (filename.find(".geom") != std::string_view::npos)
        return GLSLANG_STAGE_GEOMETRY;
    if (filename.find(".comp") != std::string_view::npos)
        return GLSLANG_STAGE_COMPUTE;
    if (filename.find(".tesc") != std::string_view::npos)
        return GLSLANG_STAGE_TESSCONTROL;
    if (filename.find(".tese") != std::string_view::npos)
        return GLSLANG_STAGE_TESSEVALUATION;

    VOX_ASSERT(false, "Unknown shader filename extension : {}", filename);
    return GLSLANG_STAGE_VERTEX;
}

glslang_stage_t GlslangUtil::VulkanStageToGlslangStage(
    VkShaderStageFlagBits vkStage)
{
    switch (vkStage)
    {
        case VK_SHADER_STAGE_VERTEX_BIT:
            return GLSLANG_STAGE_VERTEX;
        case VK_SHADER_STAGE_FRAGMENT_BIT:
            return GLSLANG_STAGE_FRAGMENT;
        case VK_SHADER_STAGE_GEOMETRY_BIT:
            return GLSLANG_STAGE_GEOMETRY;
        case VK_SHADER_STAGE_COMPUTE_BIT:
            return GLSLANG_STAGE_COMPUTE;
        case VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT:
            return GLSLANG_STAGE_TESSCONTROL;
        case VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT:
            return GLSLANG_STAGE_TESSEVALUATION;
        default:
            VOX_ASSERT(false, "Unknown shader stage flags bits : {}",
                       static_cast<uint32_t>(vkStage));
            return GLSLANG_STAGE_VERTEX;
    }
}

VkShaderStageFlagBits GlslangUtil::GlslangStageToVulkanStage(
    glslang_stage_t glslangStage)
{
    switch (glslangStage)
    {
        case GLSLANG_STAGE_VERTEX:
            return VK_SHADER_STAGE_VERTEX_BIT;
        case GLSLANG_STAGE_FRAGMENT:
            return VK_SHADER_STAGE_FRAGMENT_BIT;
        case GLSLANG_STAGE_GEOMETRY:
            return VK_SHADER_STAGE_GEOMETRY_BIT;
        case GLSLANG_STAGE_COMPUTE:
            return VK_SHADER_STAGE_COMPUTE_BIT;
        case GLSLANG_STAGE_TESSCONTROL:
            return VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case GLSLANG_STAGE_TESSEVALUATION:
            return VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        default:
            VOX_ASSERT(false, "Unknown shader stage flags bits : {}",
                       static_cast<uint32_t>(glslangStage));
            return VK_SHADER_STAGE_VERTEX_BIT;
    }
}

bool GlslangUtil::ReadShaderFile(const char* filename, std::vector<char>* dst)
{
    std::ifstream file(filename, std::ios::in | std::ios::ate);
    if (!file.is_open())
    {
        spdlog::error("Failed to find shader file {}", filename);
        return false;
    }

    const size_t fileSize = file.tellg();
    dst->resize(fileSize);

    file.seekg(std::ios::beg);
    file.read(dst->data(), static_cast<std::streamsize>(fileSize));

    file.close();
    return true;
}

bool GlslangUtil::CompileShader(glslang_stage_t stage, const char* shaderSource,
                                std::vector<unsigned int>* pSpirvBinary)
{
    constexpr TBuiltInResource defaultResource = GetDefaultGlslangResource();

    const glslang_input_t input = {
        .language = GLSLANG_SOURCE_GLSL,
        .stage = stage,
        .client = GLSLANG_CLIENT_VULKAN,
        .client_version = GLSLANG_TARGET_VULKAN_1_3,
        .target_language = GLSLANG_TARGET_SPV,
        .target_language_version = GLSLANG_TARGET_SPV_1_3,
        .code = shaderSource,
        .default_version = 100,
        .default_profile = GLSLANG_NO_PROFILE,
        .force_default_version_and_profile = false,
        .forward_compatible = false,
        .messages = GLSLANG_MSG_DEFAULT_BIT,
        .resource =
            reinterpret_cast<const glslang_resource_t*>(&defaultResource),
    };

    glslang_shader_t* shader = glslang_shader_create(&input);

    if (!glslang_shader_preprocess(shader, &input))
    {
        spdlog::error("[Glslang] GLSL preprocessing failed\n{}\n{}",
                      glslang_shader_get_info_log(shader),
                      glslang_shader_get_info_debug_log(shader));
        return false;
    }

    if (!glslang_shader_parse(shader, &input))
    {
        spdlog::error("[Glslang] GLSL parsing failed\n{}\n{}",
                      glslang_shader_get_info_log(shader),
                      glslang_shader_get_info_debug_log(shader));
        return false;
    }

    glslang_program_t* program = glslang_program_create();
    glslang_program_add_shader(program, shader);

    if (!glslang_program_link(
            program, GLSLANG_MSG_SPV_RULES_BIT | GLSLANG_MSG_VULKAN_RULES_BIT))
    {
        spdlog::error("[Glslang] GLSL linking failed\n{}\n{}",
                      glslang_shader_get_info_log(shader),
                      glslang_shader_get_info_debug_log(shader));
        return false;
    }

    glslang_program_SPIRV_generate(program, stage);

    pSpirvBinary->resize(glslang_program_SPIRV_get_size(program));
    glslang_program_SPIRV_get(program, pSpirvBinary->data());

    {
        if (const char* spirvMessages =
                glslang_program_SPIRV_get_messages(program))
        {
            spdlog::error("[Glslang] {}", spirvMessages);
        }
    }

    glslang_program_delete(program);
    glslang_shader_delete(shader);
    return true;
}
}  // namespace VoxFlow