// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandPool.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/RenderDevice.hpp>

namespace VoxFlow
{

RenderDevice::RenderDevice(Context deviceSetupCtx)
{
    deviceSetupCtx.addRequiredQueue(
        "GCT",
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1,
        1.0F, true);
    deviceSetupCtx.addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                                        false);

    _deviceSetupCtx = new Context(deviceSetupCtx);
    _instance = new Instance(deviceSetupCtx);
    _physicalDevice = new PhysicalDevice(_instance);

    // TODO(snowapril) : support multiple logical devices
    _logicalDevices.emplace_back(
        new LogicalDevice(deviceSetupCtx, _physicalDevice, _instance));

    _mainQueue = _logicalDevices[0]->getQueuePtr("GCT");
    _mainCommandPool = new CommandPool(_logicalDevices[0], _mainQueue);

    for (uint32_t f = 0; f < FRAME_BUFFER_COUNT; ++f)
    {
        _mainCommandBuffers.push_back(
            _mainCommandPool->allocateCommandBuffer());
    }
}

RenderDevice::~RenderDevice()
{
    delete _mainCommandPool;
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

void RenderDevice::beginFrame(const uint32_t deviceIndex)
{
    VOX_ASSERT(deviceIndex < _logicalDevices.size(),
               "Given Index(%u), Num LogicalDevices(%u)", deviceIndex,
               _logicalDevices.size());

    _logicalDevices[deviceIndex]->executeOnEachSwapChain(
        [this](std::shared_ptr<SwapChain> swapChain) {
            const uint32_t currentFrameIndex = swapChain->getFrameIndex();
            swapChain->waitForGpuComplete(currentFrameIndex);

            swapChain->acquireNextImageIndex();

            _mainCommandBuffers[currentFrameIndex]->beginCommandBuffer(
                swapChain->getSwapChainIndex(), currentFrameIndex,
                "MainRendering");
            _mainCommandBuffers[currentFrameIndex]->endCommandBuffer();
            _mainQueue->submitCommandBuffer(_mainCommandBuffers[currentFrameIndex], swapChain,
                                            currentFrameIndex, false);
        });
}

void RenderDevice::presentSwapChains(const uint32_t deviceIndex)
{
    VOX_ASSERT(deviceIndex < _logicalDevices.size(),
               "Given Index(%u), Num LogicalDevices(%u)", deviceIndex,
               _logicalDevices.size());

    _logicalDevices[deviceIndex]->executeOnEachSwapChain(
        [](std::shared_ptr<SwapChain> swapChain) {
            swapChain->present();
            swapChain->prepareForNextFrame();
        });
}

}  // namespace VoxFlow