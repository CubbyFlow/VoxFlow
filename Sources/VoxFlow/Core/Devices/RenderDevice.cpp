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
    deviceSetupCtx.addRequiredQueue("ImmediateUpload", VK_QUEUE_TRANSFER_BIT, 1,
                                    1.0F, false);

    deviceSetupCtx.addInstanceExtension(VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
                                        false);
    deviceSetupCtx.addDeviceExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
    deviceSetupCtx.addDeviceExtension(
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

    _deviceSetupCtx = new Context(deviceSetupCtx);
    _instance = new Instance(deviceSetupCtx);
    _physicalDevice = new PhysicalDevice(_instance);

    _logicalDevices.emplace_back(std::make_unique<LogicalDevice>(
        deviceSetupCtx, _physicalDevice, _instance,
        LogicalDeviceType::MainDevice));

    LogicalDevice* mainLogicalDevice =
        getLogicalDevice(LogicalDeviceType::MainDevice);
    _mainSwapChain = mainLogicalDevice->addSwapChain("VoxFlow Editor",
                                                     glm::ivec2(1280, 920));

    _sceneRenderer =
        std::make_unique<SceneRenderer>(mainLogicalDevice, &_frameGraph);

    _mainCmdJobSystem = mainLogicalDevice->getCommandJobSystem();

    _uploadContext = new ResourceUploadContext(this);

    Thread::SetThreadName("MainThread");
}

RenderDevice::~RenderDevice()
{
    _logicalDevices.clear();

    delete _physicalDevice;
    delete _instance;
    delete _deviceSetupCtx;
}

void RenderDevice::initializePasses()
{
    _sceneRenderer->initializePasses();
}

void RenderDevice::updateRender(const double deltaTime)
{
    SCOPED_CHROME_TRACING("RenderDevice::updateRender");
    (void)deltaTime;

    const CommandStreamKey cmdStreamKey = { ._cmdStreamName =
                                                ASYNC_UPLOAD_STREAM_NAME,
                                            ._cmdStreamUsage =
                                                CommandStreamUsage::Transfer };

    CommandStream* asyncUploadStream =
        _mainCmdJobSystem->getCommandStream(cmdStreamKey);

    _sceneRenderer->updateRender(_uploadContext);

    _uploadContext->processPendingUploads(UploadPhase::PreUpdate,
                                          asyncUploadStream);

    asyncUploadStream->flush(nullptr, nullptr, false);
}

void RenderDevice::renderScene()
{
    SCOPED_CHROME_TRACING("RenderDevice::renderScene");

    const CommandStreamKey cmdStreamKey = { ._cmdStreamName =
                                                ASYNC_UPLOAD_STREAM_NAME,
                                            ._cmdStreamUsage =
                                                CommandStreamUsage::Transfer };

    CommandStream* asyncUploadStream =
        _mainCmdJobSystem->getCommandStream(cmdStreamKey);

    _uploadContext->processPendingUploads(UploadPhase::PreRender,
                                          asyncUploadStream);

    asyncUploadStream->flush(nullptr, nullptr, false);

    _mainSwapChain->prepareForNextFrame();

    std::optional<uint32_t> backBufferIndex =
        _mainSwapChain->acquireNextImageIndex();

    if (backBufferIndex.has_value())
    {
        const FrameContext tempFrameContext = {
            ._swapChainIndex = _mainSwapChain->getSwapChainIndex(),
            ._frameIndex = _mainSwapChain->getFrameIndex(),
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