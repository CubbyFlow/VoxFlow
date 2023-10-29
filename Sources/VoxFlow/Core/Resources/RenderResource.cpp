// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Resources/RenderResource.hpp>

namespace VoxFlow
{
RenderResource::RenderResource(std::string_view&& debugName, LogicalDevice* logicalDevice, RenderResourceMemoryPool* renderResourceMemoryPool)
    : _debugName(std::move(debugName)),
      _logicalDevice(logicalDevice),
      _renderResourceMemoryPool(renderResourceMemoryPool),
      _deviceType(logicalDevice->getDeviceType())
{
}

}  // namespace VoxFlow