// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraphRenderPass.hpp>
#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
namespace RenderGraph
{
static FrameGraphTexture::Usage convertAttachmentFlagsToUsage(
    FrameGraphRenderPass::ImportedDescriptor&& importedDesc)
{
    (void)importedDesc;
    // TODO(snowapril)
    return FrameGraphTexture::Usage::RenderTarget;
}

VirtualResource::VirtualResource(std::string&& name)
    : _resourceName(std::move(name))
{
}
VirtualResource ::~VirtualResource()
{
}

void VirtualResource::isReferencedByPass(PassNode* passNode)
{
    _refCount++;
    _firstPass = (_firstPass == nullptr) ? passNode : _firstPass;
    _lastPass = passNode;
}

ImportedRenderTarget::ImportedRenderTarget(
    std::string&& name, FrameGraphTexture::Descriptor&& resourceArgs,
    typename FrameGraphRenderPass::ImportedDescriptor&& importedDesc,
    const FrameGraphTexture& resource, TextureView* textureView)
    : ImportedResource<FrameGraphTexture>(
          std::move(name), std::move(resourceArgs),
          convertAttachmentFlagsToUsage(std::move(importedDesc)), resource),
      _textureViewHandle(textureView)
{
}

ImportedRenderTarget::~ImportedRenderTarget()
{
}

ResourceNode::ResourceNode(DependencyGraph* dependencyGraph,
                           ResourceHandle resourceHandle)
    : DependencyGraph::Node(dependencyGraph), _resourceHandle(resourceHandle)
{
}

}  // namespace RenderGraph

}  // namespace VoxFlow