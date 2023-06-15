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
     public:
        Node(DependencyGraph* ownerGraph);
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
                rhs._nodeId = UINT32_MAX;
            }
            return *this;
        }

     public:
        inline NodeID getNodeID() const
        {
            return _nodeId;
        }

     private:
        DependencyGraph* _ownerGraph = nullptr;
        NodeID _nodeId = UINT32_MAX;
    };

    class Edge : private NonCopyable
    {
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

     public:
        inline bool isCulled() const
        {
            return _refCount == 0;
        }

     private:
        DependencyGraph* _ownerGraph = nullptr;
        NodeID _fromNodeID = UINT32_MAX;
        NodeID _toNodeID = UINT32_MAX;
        uint32_t _refCount = 0;
    };

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
        }
        return *this;
    }

 public:
    uint32_t getNextNodeID();

 private:
    std::vector<Node*> _nodes;
    std::vector<Edge*> _edges;
    std::atomic<uint32_t> _nextNodeID;
};

}  // namespace VoxFlow

#endif