// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphResources.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>

namespace VoxFlow
{

namespace FrameGraph
{

FrameGraphPassBase::FrameGraphPassBase()
{
}

FrameGraphPassBase::~FrameGraphPassBase()
{
}

PassNode::PassNode(FrameGraph* ownerFrameGraph, std::string_view&& passName)
    : DependencyGraph::Node(ownerFrameGraph->getDependencyGraph()),
      _passName(passName)
{
}

PassNode ::~PassNode()
{
}

PassNode::PassNode(PassNode&& passNode)
    : DependencyGraph::Node(std::move(passNode))
{
    operator=(std::move(passNode));
}

PassNode& PassNode::operator=(PassNode&& passNode)
{
    if (this != &passNode)
    {
        _passName = std::move(passNode._passName);
        _hasSideEffect = passNode._hasSideEffect;
    }

    DependencyGraph::Node::operator=(std::move(passNode));
    return *this;
}

void PassNode::registerResource(FrameGraph* frameGraph, ResourceHandle resourceHandle)
{
    VirtualResource* resource = frameGraph->getVirtualResource(resourceHandle);
    resource->isReferencedByPass(this);
    _declaredHandles.insert(resourceHandle);
}

void PassNode::addDevirtualize(VirtualResource* resource)
{
    _devirtualizes.push_back(resource);
}

void PassNode::addDestroy(VirtualResource* resource)
{
    _destroyes.push_back(resource);
}

void PassNode::resolve(FrameGraph* frameGraph)
{
    (void)frameGraph;
}

RenderPassNode::RenderPassNode(FrameGraph* ownerFrameGraph, std::string_view&& passName,
                   std::unique_ptr<FrameGraphPassBase>&& pass)
    : PassNode(ownerFrameGraph, std::move(passName)), _passImpl(std::move(pass))
{
}

RenderPassNode::~RenderPassNode()
{
    _passImpl.reset();
}

RenderPassNode::RenderPassNode(RenderPassNode&& passNode)
    : PassNode(std::move(passNode))
{
    operator=(std::move(passNode));
}

RenderPassNode& RenderPassNode::operator=(RenderPassNode&& passNode)
{
    if (this != &passNode)
    {
        _passImpl.swap(passNode._passImpl);
    }

    PassNode::operator=(std::move(passNode));
    return *this;
}

void RenderPassNode::execute(const FrameGraphResources* resources,
                             CommandStream* cmdStream)
{
    _passImpl->execute(resources, cmdStream);
}

ResourceHandle RenderPassNode::declareRenderPass(
    FrameGraph* frameGraph, FrameGraphBuilder* builder, std::string_view&& name,
    typename FrameGraphRenderPass::Descriptor&& descriptor)
{
    // TODO(snowapril) : implement
    (void)frameGraph;
    (void)builder;

    const ResourceHandle rpID =
        static_cast<ResourceHandle>(_renderPassData.size());
    _renderPassData.emplace_back(std::move(name), std::move(descriptor));

    return rpID;
}

PresentPassNode::PresentPassNode(FrameGraph* ownerFrameGraph,
                                 std::string_view&& passName,
                                 SwapChain* swapChainToPresent,
                                 const FrameContext& frameContext)
    : PassNode(ownerFrameGraph, std::move(passName)),
      _swapChainToPresent(swapChainToPresent),
      _frameContext(frameContext)
{
}

PresentPassNode ::~PresentPassNode()
{
}

PresentPassNode::PresentPassNode(PresentPassNode&& passNode)
    : PassNode(std::move(passNode))
{
    operator=(std::move(passNode));
}

PresentPassNode& PresentPassNode::operator=(PresentPassNode&& passNode)
{
    PassNode::operator=(std::move(passNode));
    return *this;
}

void PresentPassNode::execute(const FrameGraphResources* resources,
                              CommandStream* cmdStream)
{
    cmdStream->addJob(CommandJobType::MakeSwapChainFinalLayout,
                      _swapChainToPresent, _frameContext._backBufferIndex);

    FenceObject executedFence =
        cmdStream->flush(_swapChainToPresent, &_frameContext, false);
    
    resources->getFrameGraph()->setLastSubmitFence(executedFence);
}

}  // namespace FrameGraph

}  // namespace VoxFlow