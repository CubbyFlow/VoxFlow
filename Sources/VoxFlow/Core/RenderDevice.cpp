// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/RenderDevice.hpp>

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

bool RenderDevice::addSwapChain(const char* title, const glm::ivec2 resolution)
{
    std::shared_ptr<SwapChain> swapChain = std::make_shared<SwapChain>(
        _instance, _physicalDevice, _logicalDevices[0],
        _logicalDevices[0]->getQueuePtr("GCT"), title, resolution);

    VOX_ASSERT(swapChain->create(), "Failed to create swapchain (name : %s)",
               title);

    return true;
}

}  // namespace VoxFlow