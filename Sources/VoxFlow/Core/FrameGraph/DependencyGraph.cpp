// Author : snowapril

#include <VoxFlow/Core/FrameGraph/DependencyGraph.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <stack>

namespace VoxFlow
{
DependencyGraph::Node::Node(DependencyGraph* ownerGraph) : _ownerGraph(ownerGraph), _nodeId(ownerGraph->getNextNodeID())
{
    _ownerGraph->registerNode(this, _nodeId);
}

DependencyGraph::Edge::Edge(DependencyGraph* ownerGraph, Node* from, Node* to)
    : _ownerGraph(ownerGraph), _fromNodeID(from->getNodeID()), _toNodeID(to->getNodeID())
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
    std::copy_if(_edges.begin(), _edges.end(), std::back_inserter(incomingEdges), [id](Edge* edge) { return edge->_toNodeID == id; });

    return incomingEdges;
}

DependencyGraph::EdgeContainer DependencyGraph::getOutgoingEdges(NodeID id)
{
    EdgeContainer outgoingEdges;
    std::copy_if(_edges.begin(), _edges.end(), std::back_inserter(outgoingEdges), [id](Edge* edge) { return edge->_fromNodeID == id; });

    return outgoingEdges;
}

void DependencyGraph::registerNode(Node* node, NodeID id)
{
    VOX_ASSERT(id == static_cast<NodeID>(_nodes.size()), "Invalid Node ID {} was given", id);
    _nodes.push_back(node);
}

void DependencyGraph::cullUnreferencedNodes()
{
    for (Edge* edge : _edges)
    {
        Node* node = getNode(edge->_fromNodeID);
        if (node->_refCount < UINT32_MAX)
        {
            ++node->_refCount;
        }
    }

    std::stack<Node*> unreferencedNodes;
    for (Node* node : _nodes)
    {
        if (node->_refCount == 0)
        {
            unreferencedNodes.push(node);
        }
    }

    while (unreferencedNodes.empty() == false)
    {
        Node* node = unreferencedNodes.top();
        unreferencedNodes.pop();

        EdgeContainer incomingEdges = getIncomingEdges(node->getNodeID());
        for (Edge* incomingEdge : incomingEdges)
        {
            Node* linkedNode = getNode(incomingEdge->_fromNodeID);
            VOX_ASSERT(linkedNode->_refCount > 0, "Reference count must not be zero");
            if ((--linkedNode->_refCount) == 0)
            {
                unreferencedNodes.push(linkedNode);
            }
        }
    }
}

void DependencyGraph::insertNode(Node* node, NodeID id)
{
    VOX_ASSERT(_nodes.size() == static_cast<size_t>(id), "Invalid NodeID");
    _nodes.push_back(node);
}

bool DependencyGraph::isEdgeValid(const Edge* edge) const
{
    return (_nodes[edge->_fromNodeID]->isCulled() == false) && (_nodes[edge->_toNodeID]->isCulled() == false);
}

}  // namespace VoxFlow