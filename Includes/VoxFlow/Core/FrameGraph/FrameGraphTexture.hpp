// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_TEXTURE_HPP
#define VOXEL_FLOW_FRAME_GRAPH_TEXTURE_HPP

#include <VoxFlow/Core/Utils/Handle.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string>

namespace VoxFlow
{

class RenderResourceAllocator;
class Texture;

namespace FrameGraph
{
struct FrameGraphTexture
{
    struct Descriptor
    {
        uint32_t _width = 0;
        uint32_t _height = 0;
        uint32_t _depth = 0;
        uint8_t _level = 0;
        uint8_t _sampleCounts = 0;
        VkFormat _format = VK_FORMAT_UNDEFINED;
    };

    using Usage = TextureUsage;

    bool create(RenderResourceAllocator* resourceAllocator,
                std::string&& debugName, Descriptor descriptor, Usage usage);

    void release(RenderResourceAllocator* resourceAllocator);

    Handle<Texture> _texture;
};
}  // namespace FrameGraph

}  // namespace VoxFlow

#endif