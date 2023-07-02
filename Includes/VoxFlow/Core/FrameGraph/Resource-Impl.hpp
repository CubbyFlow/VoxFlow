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
Resource<ResourceDataType>::Resource(
    const ResourceDataType& resource,
    ResourceDataType::Descriptor&& resourceArgs)
    : _resource(resource), _descriptor(resourceArgs), _isImportedResource(true)
{
}

template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::Resource(
    Resource<ResourceDataType>&& rhs)
{
    operator=(std::move(rhs));
}

template <ResourceConcept ResourceDataType>
typename Resource<ResourceDataType>&
Resource<ResourceDataType>::operator=(
    Resource<ResourceDataType>&& rhs)
{
    if (this != &rhs)
    {
        _resource = rhs._resource;
        _descriptor = rhs._descriptor;
        _producerPassNode = rhs._producerPassNode;
        _isImportedResource = rhs._isImportedResource;
    }

    return *this;
}

template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::~Resource()
{
}
}  // namespace FrameGraph

}  // namespace VoxFlow

#endif