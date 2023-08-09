// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderer.hpp>
#include <VoxFlow/Core/Devices/RenderDevice.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Resources/RenderResourceGarbageCollector.hpp>
#include <VoxFlow/Core/Resources/ResourceUploadContext.hpp>
#include <VoxFlow/Core/Utils/ChromeTracer.hpp>

namespace VoxFlow
{
RenderDevice::RenderDevice(Context deviceSetupCtx)
{
    deviceSetupCtx.addRequiredQueue("MainGraphics", VK_QUEUE_GRAPHICS_BIT, 1,
                                    1.0F, true);
    deviceSetupCtx.addRequiredQueue("AsyncUpload", VK_QUEUE_TRANSFER_BIT, 1,
                                    1.0F, false);
    deviceSetupCtx.addRequiredQueue("AsyncCompute", VK_QUEUE_COMPUTE_BIT, 1,
                                    1.0F, false);

    deviceSetupCtx.addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                                        false);
    deviceSetupCtx.addDeviceExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
    deviceSetupCtx.addDeviceExtension(
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

    _deviceSetupCtx = new Context(deviceSetupCtx);
    _instance = new Instance(deviceSetupCtx);
    _physicalDevice = new PhysicalDevice(_instance);

    // TODO(snowapril) : support multiple logical devices
    _logicalDevices.emplace_back(std::make_unique<LogicalDevice>(
        deviceSetupCtx, _physicalDevice, _instance));
    _mainSwapChain = _logicalDevices[0]->addSwapChain("VoxFlow Editor",
                                                      glm::ivec2(1280, 920));

    _commandJobSystem = std::make_unique<CommandJobSystem>(this);
    _sceneRenderer = std::make_unique<SceneRenderer>(
        _logicalDevices[0].get(), &_frameGraph, _commandJobSystem.get());

    Thread::SetThreadName("MainThread");

    initCommandStreams();
}

RenderDevice::~RenderDevice()
{
    _logicalDevices.clear();

    delete _physicalDevice;
    delete _instance;
    delete _deviceSetupCtx;
}

void RenderDevice::initCommandStreams()
{
    LogicalDevice* logicalDevice = _logicalDevices[0].get();

    _commandJobSystem->createCommandStream(
        CommandStreamKey{ ._cmdStreamName = MAIN_GRAPHICS_STREAM_NAME,
                          ._cmdStreamUsage = CommandStreamUsage::Graphics },
        logicalDevice, logicalDevice->getQueuePtr("MainGraphics"));

    _commandJobSystem->createCommandStream(
        CommandStreamKey{ ._cmdStreamName = ASYNC_COMPUTE_STREAM_NAME,
                          ._cmdStreamUsage = CommandStreamUsage::Compute },
        logicalDevice, logicalDevice->getQueuePtr("AsyncCompute"));

    _commandJobSystem->createCommandStream(
        CommandStreamKey{ ._cmdStreamName = ASYNC_UPLOAD_STREAM_NAME,
                          ._cmdStreamUsage = CommandStreamUsage::Transfer },
        logicalDevice, logicalDevice->getQueuePtr("AsyncUpload"));
}

void RenderDevice::updateRender(const double deltaTime)
{
    (void)deltaTime;

    const CommandStreamKey cmdStreamKey = { ._cmdStreamName =
                                                ASYNC_UPLOAD_STREAM_NAME,
                                            ._cmdStreamUsage =
                                                CommandStreamUsage::Transfer };

    CommandStream* asyncUploadStream =
        _commandJobSystem->getCommandStream(cmdStreamKey);

    SCOPED_CHROME_TRACING("RenderDevice::updateRender");
    for (const std::unique_ptr<LogicalDevice>& logicalDevice : _logicalDevices)
    {
        ResourceUploadContext* uploadContext =
            logicalDevice->getResourceUploadContext();

        uploadContext->processPendingUploads(UploadPhase::PreUpdate,
                                             asyncUploadStream);
    }
}

void RenderDevice::renderScene()
{
    SCOPED_CHROME_TRACING("RenderDevice::renderScene");

    const CommandStreamKey cmdStreamKey = { ._cmdStreamName =
                                                ASYNC_UPLOAD_STREAM_NAME,
                                            ._cmdStreamUsage =
                                                CommandStreamUsage::Transfer };

    CommandStream* asyncUploadStream =
        _commandJobSystem->getCommandStream(cmdStreamKey);

    for (const std::unique_ptr<LogicalDevice>& logicalDevice : _logicalDevices)
    {
        ResourceUploadContext* uploadContext =
            logicalDevice->getResourceUploadContext();

        uploadContext->processPendingUploads(UploadPhase::PreRender,
                                             asyncUploadStream);
    }

    std::optional<uint32_t> backBufferIndex =
        _mainSwapChain->acquireNextImageIndex();

    if (backBufferIndex.has_value())
    {
        // TODO(snowapril) :
        FrameContext tempFrameContext = {
            ._swapChainIndex = 0,
            ._frameIndex = 0,
            ._backBufferIndex = backBufferIndex.value(),
        };

        waitForRenderReady(0);

        _sceneRenderer->beginFrameGraph(tempFrameContext);

        tf::Future<void> resolveFence =
            _sceneRenderer->resolveSceneRenderPasses(_mainSwapChain.get());
        resolveFence.wait();

        _sceneRenderer->submitFrameGraph();

        _mainSwapChain->present();
    }
    else
    {
        // TODO(snowapril) : recreate swapchain
    }
}

void RenderDevice::waitForRenderReady(const uint32_t frameIndex)
{
    SCOPED_CHROME_TRACING("RenderDevice::waitForRenderReady");
    (void)frameIndex;
}

}  // namespace VoxFlow