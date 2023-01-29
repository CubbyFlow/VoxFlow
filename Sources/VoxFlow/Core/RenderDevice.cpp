// Author : snowapril

#include <VoxFlow/Core/RenderDevice.hpp>
#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>

namespace VoxFlow
{

RenderDevice::RenderDevice(Context deviceSetupCtx)
{
    _deviceSetupCtx = new Context(deviceSetupCtx);
    _instance = new Instance(deviceSetupCtx);
    _physicalDevice = new PhysicalDevice(*_instance);

    // TODO(snowapril) : support multiple logical devices
    _logicalDevices.emplace_back(
        new LogicalDevice(deviceSetupCtx, *_physicalDevice));
}

RenderDevice::~RenderDevice()
{
    for (LogicalDevice* logicalDevice : _logicalDevices)
    {
        if (logicalDevice != nullptr)
            delete logicalDevice;
    }

    if (_physicalDevice != nullptr)
        delete _physicalDevice;

    if (_instance != nullptr)
        delete _instance;

    if (_deviceSetupCtx != nullptr)
        delete _deviceSetupCtx;
}

}  // namespace VoxFlow