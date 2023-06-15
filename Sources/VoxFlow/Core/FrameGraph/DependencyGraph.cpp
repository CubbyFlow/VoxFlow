// Author : snowapril

#include <VoxFlow/Core/FrameGraph/DependencyGraph.hpp>

namespace VoxFlow
{

DependencyGraph::Node::Node(DependencyGraph* ownerGraph)
    : _ownerGraph(ownerGraph), _nodeId(ownerGraph->getNextNodeID())
{
}

DependencyGraph::Edge::Edge(DependencyGraph* ownerGraph, Node* from, Node* to)
    : _ownerGraph(ownerGraph),
      _fromNodeID(from->getNodeID()),
      _toNodeID(to->getNodeID())
{
}

DependencyGraph::DependencyGraph()
{
}
DependencyGraph::~DependencyGraph()
{
}

uint32_t DependencyGraph::getNextNodeID()
{
    return _nextNodeID.fetch_add(1U);
}

}  // namespace VoxFlow