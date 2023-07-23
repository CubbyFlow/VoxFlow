// Author : snowapril

#ifndef VOXEL_FLOW_RESOURCE_BINDING_LAYOUT_HPP
#define VOXEL_FLOW_RESOURCE_BINDING_LAYOUT_HPP

#include <volk/volk.h>
#include <array>
#include <vector>
#include <unordered_map>
#include <string>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>

namespace VoxFlow
{
class BindableResourceView;

struct ShaderVariable
{
    BindableResourceView* _view = nullptr;
    DescriptorInfo _info;
};

struct DescriptorSetLayoutBinding
{

};

struct PipelineLayoutBinding
{
};

}  // namespace VoxFlow

#endif