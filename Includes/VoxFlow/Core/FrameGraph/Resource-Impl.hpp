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
    typename ResourceDataType::Descriptor&& resourceArgs,
    typename ResourceDataType::Usage usage)
    : VirtualResource(std::move(debugName)),
      _descriptor(resourceArgs),
      _usage(usage)
{
}

template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::Resource(
    std::string&& debugName,
    typename ResourceDataType::Descriptor&& resourceArgs,
    typename ResourceDataType::Usage usage, const ResourceDataType& resource)
    : VirtualResource(std::move(debugName)),
      _descriptor(resourceArgs),
      _usage(usage),
      _resource(resource)
{
}

template <ResourceConcept ResourceDataType>
Resource<ResourceDataType>::~Resource()
{
}

template <ResourceConcept ResourceDataType>
ImportedResource<ResourceDataType>::ImportedResource(
    std::string&& debugName,
    typename ResourceDataType::Descriptor&& resourceArgs,
    typename ResourceDataType::Usage usage, const ResourceDataType& resource)
    : Resource<ResourceDataType>(std::move(debugName), std::move(resourceArgs),
                                 usage, resource)
{
}

template <ResourceConcept ResourceDataType>
ImportedResource<ResourceDataType>::~ImportedResource()
{
}

}  // namespace RenderGraph

}  // namespace VoxFlow

#endif