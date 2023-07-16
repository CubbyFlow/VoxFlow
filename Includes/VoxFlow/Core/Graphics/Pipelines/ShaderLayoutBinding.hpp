// Author : snowapril

#ifndef VOXEL_FLOW_SHADER_LAYOUT_BINDING_HPP
#define VOXEL_FLOW_SHADER_LAYOUT_BINDING_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <array>
#include <vector>

namespace VoxFlow
{
struct ShaderLayoutBinding
{
    struct VertexInputLayout
    {
        uint32_t _location = 0;
        uint32_t _binding = 0;
        uint32_t _stride = 0;
        VkFormat _format = VK_FORMAT_UNDEFINED;
    };
    std::array<DescriptorSetLayoutDesc, MAX_NUM_SET_SLOTS> _sets{};
    std::vector<VertexInputLayout> _stageInputs;
    std::vector<VertexInputLayout> _stageOutputs;
    uint32_t _pushConstantSize = 0;
};
}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::ShaderLayoutBinding>
{
    std::size_t operator()(
        VoxFlow::ShaderLayoutBinding const& shaderLayout) const noexcept;
};

#endif