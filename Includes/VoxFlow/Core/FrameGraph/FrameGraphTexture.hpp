// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_TEXTURE_HPP
#define VOXEL_FLOW_FRAME_GRAPH_TEXTURE_HPP

#include <VoxFlow/Core/Resources/Handle.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string_view>

namespace VoxFlow
{

class RenderResourceAllocator;
class Texture;

namespace RenderGraph
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

    bool create(RenderResourceAllocator* resourceAllocator, std::string&& debugName, Descriptor descriptor, Usage usage);

    void destroy(RenderResourceAllocator* resourceAllocator);

    std::shared_ptr<Texture> _texture;
    TextureView* _textureView = nullptr;
};
}  // namespace RenderGraph

}  // namespace VoxFlow

#endif