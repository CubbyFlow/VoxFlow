// Author : snowapril

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
namespace RenderGraph
{

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
    const FrameGraphTexture& resource, TextureView* textureView)
    : ImportedResource<FrameGraphTexture>(std::move(name),
                                          std::move(resourceArgs), resource),
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