// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCE_IMPL_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCE_IMPL_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>

namespace VoxFlow
{
namespace RenderGraph
{
template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::Resource(
    std::string&& name, typename ResourceDataType::Descriptor&& resourceArgs)
    : VirtualResource(std::move(name)),
      _descriptor(resourceArgs),
      _usage(static_cast<ResourceDataType::Usage>(0))
{
}
template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::Resource(
    std::string&& name, typename ResourceDataType::Descriptor&& resourceArgs,
    const ResourceDataType& resource)
    : VirtualResource(std::move(name)),
      _descriptor(resourceArgs),
      _resource(resource),
      _usage(static_cast<ResourceDataType::Usage>(0))
{
}
template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::Resource(
    std::string&& name, typename ResourceDataType::Descriptor&& resourceArgs,
    typename ResourceDataType::Usage usage)
    : VirtualResource(std::move(name)), _descriptor(resourceArgs), _usage(usage)
{
}
template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::Resource(
    std::string&& name, typename ResourceDataType::Descriptor&& resourceArgs,
    typename ResourceDataType::Usage usage, const ResourceDataType& resource)
    : VirtualResource(std::move(name)),
      _descriptor(resourceArgs),
      _resource(resource),
      _usage(usage)
{
}
}  // namespace RenderGraph

}  // namespace VoxFlow

#endif