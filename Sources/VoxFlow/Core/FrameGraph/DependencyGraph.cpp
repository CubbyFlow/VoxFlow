// Author : snowapril

#include <VoxFlow/Core/FrameGraph/DependencyGraph.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

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

DependencyGraph::NodeID DependencyGraph::getNextNodeID()
{
    return _nextNodeID.fetch_add(1U);
}

DependencyGraph::EdgeContainer DependencyGraph::getIncomingEdges(NodeID id)
{
    EdgeContainer incomingEdges;
    std::copy_if(_edges.begin(), _edges.end(),
                 std::back_inserter(incomingEdges),
                 [id](Edge* edge) { return edge->_toNodeID == id; });
    
    return incomingEdges;
}

DependencyGraph::EdgeContainer DependencyGraph::getOutgoingEdges(NodeID id)
{
    EdgeContainer outgoingEdges;
    std::copy_if(_edges.begin(), _edges.end(),
                 std::back_inserter(outgoingEdges),
                 [id](Edge* edge) { return edge->_fromNodeID == id; });

    return outgoingEdges;
}

DependencyGraph::Edge* DependencyGraph::link(NodeID fromID, NodeID toID)
{
    Node* fromNode = getNode(fromID);
    Node* toNode = getNode(toID);

    Edge *edge = new Edge(this, fromNode, toNode);
    _edges.push_back(edge);

    return edge;
}

void DependencyGraph::insertNode(Node* node, NodeID id)
{
    VOX_ASSERT(_nodes.size() == static_cast<size_t>(id), "Invalid NodeID");
    _nodes.push_back(node);
}

bool DependencyGraph::isEdgeValid(const Edge* edge) const
{
    return _nodes[edge->_fromNodeID]->isCulled() == false &&
           _nodes[edge->_toNodeID]->isCulled();
}

}  // namespace VoxFlow