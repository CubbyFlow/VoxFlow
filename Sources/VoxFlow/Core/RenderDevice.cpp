// Author : snowapril

#include <VoxFlow/Core/RenderDevice.hpp>

namespace VoxFlow
{

RenderDevice::RenderDevice(Context deviceSetupCtx)
    : _deviceSetupCtx(deviceSetupCtx)
{
    _instance = new Instance(_deviceSetupCtx);
    _physicalDevice = new PhysicalDevice(*_instance);
    _logicalDevices.emplace_back(
        new LogicalDevice(_deviceSetupCtx, *_physicalDevice));
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
}

}  // namespace VoxFlow