// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>

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
PassNode::PassNode(FrameGraph* ownerFrameGraph, std::string_view&& passName,
                   std::unique_ptr<FrameGraphPassBase>&& pass)
    : DependencyGraph::Node(ownerFrameGraph->getDependencyGraph()),
      _passImpl(std::move(pass)),
      _passName(passName)
{
}
PassNode ::~PassNode()
{
    _passImpl.reset();
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
        _passImpl.swap(passNode._passImpl);
        _passName = std::move(passNode._passName);
        _hasSideEffect = passNode._hasSideEffect;
    }

    DependencyGraph::Node::operator=(std::move(passNode));
    return *this;
}

}  // namespace FrameGraph

}  // namespace VoxFlow