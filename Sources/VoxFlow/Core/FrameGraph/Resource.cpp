// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraphRenderPass.hpp>
#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <algorithm>

namespace VoxFlow
{
namespace RenderGraph
{
static FrameGraphTexture::Usage convertAttachmentFlagsToUsage(
    FrameGraphRenderPass::ImportedDescriptor&& importedDesc)
{
    (void)importedDesc;
    // TODO(snowapril)
    return FrameGraphTexture::Usage::RenderTarget;
}

ResourceEdgeBase* ResourceNode::getReaderEdgeForPassNode(const PassNode* passNode)
{
    DependencyGraph::EdgeContainer outgoings =
        _ownerGraph->getOutgoingEdges(passNode->getNodeID());

    auto iter =
        std::find_if(outgoings.begin(), outgoings.end(),
                     [passNode, this](const DependencyGraph::Edge* edge) {
                         return edge->_fromNodeID == passNode->getNodeID() &&
                                edge->_toNodeID == _nodeId;
                     });

    return iter == outgoings.end() ? nullptr
                                   : static_cast<ResourceEdgeBase*>(*iter);
}

ResourceEdgeBase* ResourceNode::getWriterEdgeForPassNode(const PassNode* passNode)
{
    DependencyGraph::EdgeContainer incomings =
        _ownerGraph->getIncomingEdges(passNode->getNodeID());

    auto iter =
        std::find_if(incomings.begin(), incomings.end(),
                     [passNode, this](const DependencyGraph::Edge* edge) {
                         return edge->_fromNodeID == passNode->getNodeID() &&
                                edge->_toNodeID == _nodeId;
                     });

    return iter == incomings.end() ? nullptr
                                   : static_cast<ResourceEdgeBase*>(*iter);
}

void ResourceNode::addOutgoingEdge(DependencyGraph::Edge* edge)
{
    _outgoingEdges.push_back(edge);
}

void ResourceNode::setIncomingEdge(DependencyGraph::Edge* edge)
{
    VOX_ASSERT(_incomingEdge == nullptr,
               "There must not be multiple writer edges");
    _incomingEdge = edge;
}

void ResourceNode::resolveResourceUsage(FrameGraph* frameGraph)
{
    VirtualResource* resource = frameGraph->getVirtualResource(_resourceHandle);
    resource->resolveUsage(_ownerGraph, _outgoingEdges, _incomingEdge);
}

VirtualResource::VirtualResource(std::string&& name)
    : _resourceName(std::move(name))
{
}
VirtualResource ::~VirtualResource()
{
}

void VirtualResource::isReferencedByPass(PassNode* passNode)
{
    _refCount++;
    _firstPass = (_firstPass == nullptr) ? passNode : _firstPass;
    _lastPass = passNode;
}

ImportedRenderTarget::ImportedRenderTarget(
    std::string&& name, FrameGraphTexture::Descriptor&& resourceArgs,
    typename FrameGraphRenderPass::ImportedDescriptor&& importedDesc,
    const FrameGraphTexture& resource, TextureView* textureView)
    : ImportedResource<FrameGraphTexture>(
          std::move(name), std::move(resourceArgs),
          convertAttachmentFlagsToUsage(std::move(importedDesc)), resource),
      _textureViewHandle(textureView)
{
}

ImportedRenderTarget::~ImportedRenderTarget()
{
}

ResourceNode::ResourceNode(DependencyGraph* dependencyGraph,
                           ResourceHandle resourceHandle)
    : DependencyGraph::Node(dependencyGraph), _resourceHandle(resourceHandle)
{
}

}  // namespace RenderGraph

}  // namespace VoxFlow