// Author : snowapril

#ifndef VOXEL_FLOW_DEPENDENCY_GRAPH_HPP
#define VOXEL_FLOW_DEPENDENCY_GRAPH_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <atomic>
#include <cstdint>
#include <vector>

namespace VoxFlow
{
class DependencyGraph : private NonCopyable
{
 public:
    using NodeID = uint32_t;

    class Node : private NonCopyable
    {
        friend class DependencyGraph;

     public:
        explicit Node(DependencyGraph* ownerGraph);
        Node(Node&& rhs)
        {
            operator=(std::move(rhs));
        }
        Node& operator=(Node&& rhs)
        {
            if (this != &rhs)
            {
                _ownerGraph = rhs._ownerGraph;
                _nodeId = rhs._nodeId;
                _refCount = rhs._refCount;
            }
            return *this;
        }

     public:
        inline NodeID getNodeID() const
        {
            return _nodeId;
        }

        inline bool isCulled() const
        {
            return _refCount == 0;
        }

        DependencyGraph* _ownerGraph = nullptr;
        NodeID _nodeId = UINT32_MAX;
        uint32_t _refCount = 0;
    };

    class Edge : private NonCopyable
    {
        friend class DependencyGraph;

     public:
        Edge(DependencyGraph* ownerGraph, Node* from, Node* to);
        Edge(Edge&& rhs)
        {
            operator=(std::move(rhs));
        }
        Edge& operator=(Edge&& rhs)
        {
            if (this != &rhs)
            {
                _ownerGraph = rhs._ownerGraph;
                _fromNodeID = rhs._fromNodeID;
                _toNodeID = rhs._toNodeID;
                rhs._fromNodeID = UINT32_MAX;
                rhs._toNodeID = UINT32_MAX;
            }
            return *this;
        }

        DependencyGraph* _ownerGraph = nullptr;
        NodeID _fromNodeID = UINT32_MAX;
        NodeID _toNodeID = UINT32_MAX;
    };

    using NodeContainer = std::vector<Node*>;
    using EdgeContainer = std::vector<Edge*>;

 public:
    DependencyGraph();
    ~DependencyGraph();
    DependencyGraph(DependencyGraph&& rhs)
    {
        operator=(std::move(rhs));
    }
    DependencyGraph& operator=(DependencyGraph&& rhs)
    {
        if (this != &rhs)
        {
            _nodes = std::move(rhs._nodes);
            _edges = std::move(rhs._edges);
            _nextNodeID.store(rhs._nextNodeID.load());
        }
        return *this;
    }

 public:
    NodeID getNextNodeID();
    Node* getNode(NodeID id)
    {
        return _nodes[id];
    }

    EdgeContainer getIncomingEdges(NodeID id);
    EdgeContainer getOutgoingEdges(NodeID id);

    void registerNode(Node* node, NodeID id);
    Edge* link(NodeID fromID, NodeID toID);
    void cullUnreferencedNodes();
    void insertNode(Node* node, NodeID id);
    bool isEdgeValid(const Edge* edge) const;

    inline const EdgeContainer& getLinkedEdges() const
    {
        return _edges;
    }

 private:
    NodeContainer _nodes;
    EdgeContainer _edges;
    std::atomic<NodeID> _nextNodeID;
};

}  // namespace VoxFlow

#endif