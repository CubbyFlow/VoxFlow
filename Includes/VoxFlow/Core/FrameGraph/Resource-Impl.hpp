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
    std::string&& debugName,
    typename ResourceDataType::Descriptor&& resourceArgs)
    : VirtualResource(std::move(debugName)), _descriptor(resourceArgs)
{
}

template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::Resource(
    std::string&& debugName,
    typename ResourceDataType::Descriptor&& resourceArgs,
    const ResourceDataType& resource)
    : VirtualResource(std::move(debugName)),
      _descriptor(resourceArgs),
      _resource(resource)
{
}

template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::~Resource()
{
}

template <ResourceConcept ResourceDataType>
ImportedResource<ResourceDataType>::ImportedResource(
    std::string&& debugName, const ResourceDataType& resource,
    typename ResourceDataType::Descriptor&& resourceArgs)
    : Resource<ResourceDataType>(std::move(debugName), std::move(resourceArgs),
                                 resource)
{
}

template <ResourceConcept ResourceDataType>
ImportedResource<ResourceDataType>::~ImportedResource()
{
}

}  // namespace RenderGraph

}  // namespace VoxFlow

#endif