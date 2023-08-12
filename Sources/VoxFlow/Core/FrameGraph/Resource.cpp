// Author : snowapril

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
namespace FrameGraph
{

VirtualResource::VirtualResource()
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
    const FrameGraphTexture& resource,
    FrameGraphTexture::Descriptor&& resourceArgs, Texture* texture)
    : ImportedResource<FrameGraphTexture>(resource, std::move(resourceArgs)),
      _textureHandle(texture)
{
    (void)_textureHandle;
}

ImportedRenderTarget::~ImportedRenderTarget()
{
}

ResourceNode::ResourceNode(DependencyGraph* dependencyGraph,
                           std::string_view&& resourceName,
                           ResourceHandle resourceHandle)
    : DependencyGraph::Node(dependencyGraph),
      _resourceName(std::move(resourceName)),
      _resourceHandle(resourceHandle)
{
    (void)_resourceHandle;
}

}  // namespace FrameGraph

}  // namespace VoxFlow