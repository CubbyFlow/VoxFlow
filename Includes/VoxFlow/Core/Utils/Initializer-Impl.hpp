// Author : snowapril

#ifndef VOXEL_FLOW_INITIALIZER_IMPL_HPP
#define VOXEL_FLOW_INITIALIZER_IMPL_HPP

#include <VoxFlow/Core/Utils/pch.hpp>

namespace VoxFlow
{
template <>
inline VkPipelineVertexInputStateCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .vertexBindingDescriptionCount = 0,
             .pVertexBindingDescriptions = nullptr,
             .vertexAttributeDescriptionCount = 0,
             .pVertexAttributeDescriptions = nullptr };
}

template <>
inline VkShaderModuleCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .codeSize = 0,
             .pCode = nullptr };
}

template <>
inline VkPipelineShaderStageCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .stage = VK_SHADER_STAGE_VERTEX_BIT,
             .module = VK_NULL_HANDLE,
             .pName = "main",
             .pSpecializationInfo = nullptr };
}

template <>
inline VkPipelineInputAssemblyStateCreateInfo Initializer::MakeInfo()
{
    return { .sType =
                 VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP,
             .primitiveRestartEnable = VK_FALSE };
}

template <>
inline VkPipelineTessellationStateCreateInfo Initializer::MakeInfo()
{
    return { .sType =
                 VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .patchControlPoints = 3 };
}

template <>
inline VkPipelineViewportStateCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .viewportCount = 0,
             .pViewports = nullptr,
             .scissorCount = 0,
             .pScissors = nullptr };
}

template <>
inline VkPipelineRasterizationStateCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .depthClampEnable = VK_FALSE,
             .rasterizerDiscardEnable = VK_FALSE,
             .polygonMode = VK_POLYGON_MODE_FILL,
             .cullMode = VK_CULL_MODE_BACK_BIT,
             .frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE,
             .depthBiasEnable = VK_FALSE,
             .depthBiasConstantFactor = 0.0f,
             .depthBiasClamp = 0.0f,
             .depthBiasSlopeFactor = 0.0f,
             .lineWidth = 1.0f };
}

template <>
inline VkPipelineMultisampleStateCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .rasterizationSamples = VK_SAMPLE_COUNT_1_BIT,
             .sampleShadingEnable = VK_FALSE,
             .minSampleShading = 1.0f,
             .pSampleMask = nullptr,
             .alphaToCoverageEnable = VK_FALSE,
             .alphaToOneEnable = VK_FALSE };
}

template <>
inline VkPipelineDepthStencilStateCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .depthTestEnable = VK_TRUE,
             .depthWriteEnable = VK_TRUE,
             .depthCompareOp = VK_COMPARE_OP_LESS,
             .depthBoundsTestEnable = VK_FALSE,
             .stencilTestEnable = VK_TRUE,
             .front = {},
             .back = {},
             .minDepthBounds = 0.0,
             .maxDepthBounds = 1.0f };
}

template <>
inline VkPipelineColorBlendStateCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .logicOpEnable = VK_FALSE,
             .logicOp = VK_LOGIC_OP_COPY,
             .attachmentCount = 0,
             .pAttachments = nullptr,
             .blendConstants = { 0.0f, 0.0f, 0.0f, 0.0f } };
}

template <>
inline VkPipelineDynamicStateCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .dynamicStateCount = 0,
             .pDynamicStates = nullptr };
}
}  // namespace VoxFlow

#include <VoxFlow/Core/Utils/Initializer-Impl.hpp>

#endif