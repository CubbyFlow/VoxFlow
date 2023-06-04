// Author : snowapril

#include <VoxFlow/Core/Resources/BindableResourceView.hpp>

namespace VoxFlow
{
BindableResourceView::BindableResourceView(std::string&& debugName,
                                           LogicalDevice* logicalDevice)
    : _debugName(debugName), _logicalDevice(logicalDevice)
{
}

BindableResourceView::~BindableResourceView()
{
}
}  // namespace VoxFlow