// Author : snowapril

#ifndef VOXEL_FLOW_INITIALIZER_IMPL_HPP
#define VOXEL_FLOW_INITIALIZER_IMPL_HPP

#include <VoxFlow/Core/Utils/pch.hpp>

namespace VoxFlow
{
template <>
VkPipelineVertexInputStateCreateInfo Initializer::MakeInfo()
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
VkShaderModuleCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO,
             .pNext = nullptr,
             .flags = 0,
             .codeSize = 0,
             .pCode = nullptr };
}

template <>
VkPipelineShaderStageCreateInfo Initializer::MakeInfo()
{
    return { .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
      .pNext = nullptr,
      .flags = 0,
      .stage = VK_SHADER_STAGE_VERTEX_BIT,
      .module = VK_NULL_HANDLE,
      .pName = "main",
      .pSpecializationInfo = nullptr };
}
}  // namespace VoxFlow

#include <VoxFlow/Core/Utils/Initializer-Impl.hpp>

#endif