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

ImportedRenderTarget::ImportedRenderTarget(
    const FrameGraphTexture& resource,
    FrameGraphTexture::Descriptor&& resourceArgs, Texture* texture)
    : ImportedResource<FrameGraphTexture>(resource, std::move(resourceArgs)),
      _textureHandle(texture)
{
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
}

}  // namespace FrameGraph

}  // namespace VoxFlow