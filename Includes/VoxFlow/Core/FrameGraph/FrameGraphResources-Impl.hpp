// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCES_IMPL_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCES_IMPL_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphResources.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{

namespace RenderGraph
{

template <ResourceConcept ResourceDataType>
[[nodiscard]] inline const Resource<ResourceDataType>&
FrameGraphResources::getResource(ResourceHandle handle) const
{
#if defined(VOXFLOW_DEBUG)
    auto declaredHandles = _passNode->getDeclaredHandles();
    VOX_ASSERT(
        declaredHandles.find(handle) != declaredHandles.end(),
        "Should not try to get resource that is not declared in this pass");
#endif

    VirtualResource* vresource = _frameGraph->getVirtualResource(handle);
    return static_cast<const Resource<ResourceDataType>&>(*vresource);
}

template <ResourceConcept ResourceDataType>
[[nodiscard]] inline const typename ResourceDataType::Descriptor&
FrameGraphResources::getResourceDescriptor(ResourceHandle handle) const
{
#if defined(VOXFLOW_DEBUG)
    auto declaredHandles = _passNode->getDeclaredHandles();
    VOX_ASSERT(
        declaredHandles.find(handle) != declaredHandles.end(),
        "Should not try to get resource that is not declared in this pass");
#endif

    return _frameGraph->getResourceDescriptor(handle);
}

}  // namespace RenderGraph

}  // namespace VoxFlow

#endif