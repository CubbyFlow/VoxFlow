// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCE_IMPL_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCE_IMPL_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>

namespace VoxFlow
{
namespace FrameGraph
{
template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::Resource(
    ResourceDataType::Descriptor&& resourceArgs)
    : _descriptor(resourceArgs)
{
}

template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::~Resource()
{
}

template <ResourceConcept ResourceDataType>
ImportedResource<ResourceDataType>::ImportedResource(
    const ResourceDataType& resource,
    ResourceDataType::Descriptor&& resourceArgs)
    : Resource<ResourceDataType>(std::move(resourceArgs)), _resource(resource)
{
}

template <ResourceConcept ResourceDataType>
ImportedResource<ResourceDataType>::~ImportedResource()
{
}

}  // namespace FrameGraph

}  // namespace VoxFlow

#endif