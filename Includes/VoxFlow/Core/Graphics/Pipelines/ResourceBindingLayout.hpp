// Author : snowapril

#ifndef VOXEL_FLOW_RESOURCE_BINDING_LAYOUT_HPP
#define VOXEL_FLOW_RESOURCE_BINDING_LAYOUT_HPP

#include <volk/volk.h>
#include <string>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>

namespace VoxFlow
{
class BindableResourceView;

struct ShaderVariableBinding
{
    std::string _variableName;
    BindableResourceView* _view = nullptr;
    ResourceLayout _usage = ResourceLayout::Undefined;
};

}  // namespace VoxFlow

#endif