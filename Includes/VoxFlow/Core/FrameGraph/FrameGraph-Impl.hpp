// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_IMPL_HPP
#define VOXEL_FLOW_FRAME_GRAPH_IMPL_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <functional>

namespace VoxFlow
{

namespace RenderGraph
{
constexpr uint32_t EXECUTION_LAMBDA_SIZE_LIMIT = 1024U;

template <ResourceConcept ResourceDataType>
ResourceHandle FrameGraphBuilder::allocate(
    std::string&& resourceName,
    typename ResourceDataType::Descriptor&& initArgs)
{
    return _frameGraph->create<ResourceDataType>(std::move(resourceName),
                                                 std::move(initArgs));
}

template <typename PassDataType, typename SetupPhase, typename ExecutePhase>
const PassDataType& FrameGraph::addCallbackPass(std::string_view&& passName,
                                                SetupPhase&& setup,
                                                ExecutePhase&& execute)
{
    static_assert(sizeof(ExecutePhase) < EXECUTION_LAMBDA_SIZE_LIMIT,
                  "ExecutePhase() lambda captures too much data");

    std::unique_ptr<FrameGraphPass<PassDataType, ExecutePhase>> pass =
        std::make_unique<FrameGraphPass<PassDataType, ExecutePhase>>(
            std::forward<ExecutePhase>(execute));

    PassDataType& passData = pass->getPassData();

    _passNodes.emplace_back(
        new RenderPassNode(this, std::move(passName), std::move(pass)));

    FrameGraphBuilder builder(this, _passNodes.back());
    std::invoke(setup, builder, passData);

    return passData;
}

template <typename SetupPhase, typename ExecutePhase>
void FrameGraph::addCallbackPass(std::string_view&& passName,
                                 SetupPhase&& setup, ExecutePhase&& execute)
{
    struct EmptyPassData {};

    [[maybe_unused]] auto _ =
        addCallbackPass<EmptyPassData, SetupPhase, ExecutePhase>(
            std::move(passName), std::move(setup), std::move(execute));
}

template <typename SetupPhase>
void FrameGraph::addPresentPass(std::string_view&& passName, SetupPhase&& setup,
                                SwapChain* swapChain,
                                const FrameContext& frameContext)
{
    _passNodes.emplace_back(new PresentPassNode(this, std::move(passName),
                                                swapChain, frameContext));

    FrameGraphBuilder builder(this, _passNodes.back());
    std::invoke(setup, builder);
    builder.setSideEffectPass();
}

template <ResourceConcept ResourceDataType>
ResourceHandle FrameGraph::create(
    std::string&& resourceName,
    typename ResourceDataType::Descriptor&& resourceDescArgs)
{
    VirtualResource* virtualResource = new Resource<ResourceDataType>(
        std::move(resourceName), std::move(resourceDescArgs));

    ResourceHandle resourceHandle(_resources.size());

    _resourceSlots.push_back(
        { ._resourceIndex =
              static_cast<ResourceSlot::IndexType>(_resourceNodes.size()),
          ._nodeIndex = static_cast<ResourceSlot::IndexType>(_resources.size()),
          ._version = static_cast<ResourceSlot::VersionType>(0) });
    _resources.push_back(virtualResource);

    ResourceNode* resourceNode =
        new ResourceNode(&_dependencyGraph, resourceHandle);
    _resourceNodes.push_back(resourceNode);

    return resourceHandle;
}

template <ResourceConcept ResourceDataType>
const typename ResourceDataType::Descriptor FrameGraph::getResourceDescriptor(
    ResourceHandle id) const
{
    const ResourceSlot& resourceSlot = getResourceSlot(id);
    auto resource = static_cast<Resource<ResourceDataType>*>(
        _resources[resourceSlot._resourceIndex]);
    return resource->getDescriptor();
}

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
[[nodiscard]] inline const typename ResourceDataType::Descriptor
FrameGraphResources::getResourceDescriptor(ResourceHandle handle) const
{
#if defined(VOXFLOW_DEBUG)
    auto declaredHandles = _passNode->getDeclaredHandles();
    VOX_ASSERT(
        declaredHandles.find(handle) != declaredHandles.end(),
        "Should not try to get resource that is not declared in this pass");
#endif

    return _frameGraph->getResourceDescriptor<ResourceDataType>(handle);
}

}  // namespace RenderGraph

}  // namespace VoxFlow

#endif