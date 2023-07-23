// Author : snowapril

#ifndef VOXEL_FLOW_RESOURCE_BINDING_LAYOUT_HPP
#define VOXEL_FLOW_RESOURCE_BINDING_LAYOUT_HPP

#include <volk/volk.h>
#include <array>
#include <vector>
#include <unordered_map>
#include <string_view>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayoutDescriptor.hpp>

namespace VoxFlow
{
class BindableResourceView;

struct ShaderVariable
{
    std::string_view _variableName;
    BindableResourceView* _view = nullptr;
    ResourceLayout _usage = ResourceLayout::Undefined;
};

struct DescriptorBinding
{
    ShaderVariable _variable;
    DescriptorInfo _info;
};

struct DescriptorSetLayoutBinding
{
    SetSlotCategory _setCategory = SetSlotCategory::Undefined;
    std::vector<DescriptorBinding> _descriptorBindings;
};

struct PipelineLayoutBinding
{
    std::array<DescriptorSetLayoutBinding,
               static_cast<uint32_t>(SetSlotCategory::Count)>
        _setBindings;
};

}  // namespace VoxFlow

#endif