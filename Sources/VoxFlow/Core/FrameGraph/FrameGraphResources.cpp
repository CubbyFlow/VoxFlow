// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphResources.hpp>

namespace VoxFlow
{
class TextureView;

namespace RenderGraph
{

TextureView* FrameGraphResources::getTextureView(ResourceHandle handle) const
{
    Resource<FrameGraphTexture>* resource =
        static_cast<Resource<FrameGraphTexture>*>(
            _frameGraph->getVirtualResource(handle));

    TextureView* attachmentView = nullptr;
    if (resource->isImported())
    {
        attachmentView =
            static_cast<ImportedRenderTarget*>(resource)->getTextureView();
    }
    else
    {
        attachmentView = resource->getInternalResource()._textureView;
    }

    return attachmentView;
}

}  // namespace RenderGraph

}  // namespace VoxFlow