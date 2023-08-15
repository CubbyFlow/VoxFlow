// Author : snowapril

#include <VoxFlow/Core/Resources/BindableResourceView.hpp>

namespace VoxFlow
{
BindableResourceView::BindableResourceView(std::string&& debugName,
                                           LogicalDevice* logicalDevice,
                                           RenderResource* ownerResource)
    : _debugName(debugName),
      _logicalDevice(logicalDevice),
      _ownerResource(ownerResource)
{
}

BindableResourceView::~BindableResourceView()
{
}
}  // namespace VoxFlow