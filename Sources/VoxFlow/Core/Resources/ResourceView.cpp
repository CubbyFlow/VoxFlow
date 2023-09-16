// Author : snowapril

#include <VoxFlow/Core/Resources/ResourceView.hpp>

namespace VoxFlow
{
ResourceView::ResourceView(std::string&& debugName,
                                           LogicalDevice* logicalDevice,
                                           RenderResource* ownerResource)
    : _debugName(debugName),
      _logicalDevice(logicalDevice),
      _ownerResource(ownerResource)
{
}

ResourceView::~ResourceView()
{
}
}  // namespace VoxFlow