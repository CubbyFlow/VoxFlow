// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCES_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCES_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>

namespace VoxFlow
{

namespace FrameGraph
{

class FrameGraph;

class FrameGraphResources
{
 public:
    explicit FrameGraphResources(FrameGraph* frameGraph, PassNode* passNode)
        : _frameGraph(frameGraph), _passNode(passNode)
    {
    }

    ~FrameGraphResources()
    {
    }

    [[nodiscard]] inline FrameGraph* getFrameGraph() const
    {
        return _frameGraph;
    }

    [[nodiscard]] inline PassNode* getPassNode() const
    {
        return _passNode;
    }

    RenderPassNode::RenderPassData const* getRenderPassData(
        ResourceHandle rpID) const
    {
        return static_cast<RenderPassNode*>(_passNode)->getRenderPassData(rpID);
    }

 private:
    FrameGraph* _frameGraph = nullptr;
    PassNode* _passNode = nullptr;
};
}  // namespace FrameGraph

}  // namespace VoxFlow

#endif