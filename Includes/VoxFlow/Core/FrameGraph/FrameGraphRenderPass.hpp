// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RENDER_PASS_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RENDER_PASS_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Utils/Handle.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string>

namespace VoxFlow
{

class RenderResourceAllocator;
class Texture;

namespace FrameGraph
{
struct FrameGraphRenderPass
{
    struct Attachment
    {
        std::array<ResourceHandle, MAX_RENDER_TARGET_COUNTS> _colors;
        ResourceHandle _depth;
        ResourceHandle _stencil;
    };

    struct Descriptor
    {
        std::vector<Attachment> _attachments;
        // TODO(snowapril) : fill below needed for render pass creation
    };
};
}  // namespace FrameGraph

}  // namespace VoxFlow

#endif