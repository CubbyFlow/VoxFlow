// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RENDER_PASS_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RENDER_PASS_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassParams.hpp>
#include <VoxFlow/Core/Utils/Handle.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <array>
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
        std::array<Attachment, MAX_RENDER_TARGET_COUNTS> _attachments;
        glm::uvec2 _viewportSize;
        std::array<glm::vec4, MAX_RENDER_TARGET_COUNTS> _clearColors;
        float _clearDepth = 0.0f;
        uint8_t _clearStencil = 0;
        AttachmentMaskFlags _clearFlags = AttachmentMaskFlags::None;
        AttachmentMaskFlags _writableAttachment = AttachmentMaskFlags::All;
        uint8_t _numSamples = 0;
    };

    uint32_t _id = 0;
};
}  // namespace FrameGraph

}  // namespace VoxFlow

#endif