// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCES_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCES_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/FrameGraph/ResourceHandle.hpp>
#include <VoxFlow/Core/FrameGraph/TypeTraits.hpp>

namespace VoxFlow
{
class TextureView;

namespace RenderGraph
{

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

    template <ResourceConcept ResourceDataType>
    [[nodiscard]] inline const Resource<ResourceDataType>& getResource(
        ResourceHandle handle) const;

    template <ResourceConcept ResourceDataType>
    [[nodiscard]] inline const typename ResourceDataType::Descriptor&
    getResourceDescriptor(ResourceHandle handle) const;

    [[nodiscard]] TextureView* getTextureView(ResourceHandle handle) const;

    RenderPassData* getRenderPassData(const uint32_t rpID) const
    {
        return static_cast<RenderPassNode*>(_passNode)->getRenderPassData(rpID);
    }

 private:
    FrameGraph* _frameGraph = nullptr;
    PassNode* _passNode = nullptr;
};
}  // namespace RenderGraph

}  // namespace VoxFlow

#include <VoxFlow/Core/FrameGraph/FrameGraphResources-Impl.hpp>

#endif