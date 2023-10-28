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

template <ResourceConcept ResourceDataType>
ResourceHandle FrameGraphBuilder::read(ResourceHandle id,
                                       typename ResourceDataType::Usage usage)
{
    return _frameGraph->readInternal(
        id, _currentPassNode,
        [this, usage](ResourceNode* node, VirtualResource* vResource) {
            Resource<ResourceDataType>* resource =
                static_cast<Resource<ResourceDataType>*>(vResource);
            return resource->connect(_frameGraph->getDependencyGraph(), node,
                                     _currentPassNode, usage);
        });
}

template <ResourceConcept ResourceDataType>
ResourceHandle FrameGraphBuilder::write(ResourceHandle id,
                                        typename ResourceDataType::Usage usage)
{
    return _frameGraph->writeInternal(
        id, _currentPassNode,
        [this, usage](ResourceNode* node, VirtualResource* vResource) {
            Resource<ResourceDataType>* resource =
                static_cast<Resource<ResourceDataType>*>(vResource);
            return resource->connect(_frameGraph->getDependencyGraph(),
                                     _currentPassNode, node, usage);
        });
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
bool Resource<ResourceDataType>::connect(DependencyGraph* dependencyGraph,
                                         ResourceNode* node, PassNode* passNode,
                                         typename ResourceDataType::Usage usage)
{
    ResourceEdge* edge =
        static_cast<ResourceEdge*>(node->getReaderEdgeForPassNode(passNode));

    if (edge == nullptr)
    {
        edge = static_cast<ResourceEdge*>(dependencyGraph->link<ResourceEdge>(
            node->getNodeID(), passNode->getNodeID(), usage));
    }
    else
    {
        *edge |= usage;
    }
    node->addOutgoingEdge(edge);

    return true;
}

template <ResourceConcept ResourceDataType>
bool Resource<ResourceDataType>::connect(DependencyGraph* dependencyGraph,
                                         PassNode* passNode, ResourceNode* node,
                                         typename ResourceDataType::Usage usage)
{
    ResourceEdge* edge =
        static_cast<ResourceEdge*>(node->getWriterEdgeForPassNode(passNode));

    if (edge == nullptr)
    {
        edge = static_cast<ResourceEdge*>(dependencyGraph->link<ResourceEdge>(
            passNode->getNodeID(), node->getNodeID(), usage));
    }
    else
    {
        *edge |= usage;
    }
    node->setIncomingEdge(edge);

    return true;
}

template <ResourceConcept ResourceDataType>
void Resource<ResourceDataType>::resolveUsage(
    DependencyGraph* dependencyGraph,
    const DependencyGraph::EdgeContainer& edges,
    DependencyGraph::Edge* writerEdge)
{
    for (DependencyGraph::Edge* edge : edges)
    {
        if (dependencyGraph->isEdgeValid(edge))
        {
            ResourceEdge* resourceEdge = static_cast<ResourceEdge*>(edge);
            _usage |= resourceEdge->getUsage();
        }
    }

    if (writerEdge != nullptr)
    {
        _usage |= static_cast<ResourceEdge*>(writerEdge)->getUsage();
    }
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