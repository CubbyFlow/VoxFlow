// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandPool.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp>
#include <VoxFlow/Core/RenderDevice.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

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

    
    auto mainSwapChain = _logicalDevices[0]->addSwapChain("VoxFlow Editor", glm::ivec2(1280, 920));

    RenderTargetLayoutKey rtLayoutKey = {
        ._debugName = "MainRenderingLayoutKey",
        ._colorAttachmentDescs = { { ._resolution = glm::ivec3(
                                         mainSwapChain->getResolution(), 1),
                                     ._format =
                                         mainSwapChain->getSurfaceFormat(),
                                     ._clearColor = true } },
        ._depthStencilAttachment = std::nullopt,
    };
    auto renderPass = _logicalDevices[0]->getRenderPassCollector()->getOrCreateRenderPass(
        rtLayoutKey);

    _mainQueue = _logicalDevices[0]->getQueuePtr("GCT");
    _mainCommandPool = new CommandPool(_logicalDevices[0], _mainQueue);

    for (uint32_t f = 0; f < FRAME_BUFFER_COUNT; ++f)
    {
        _mainCommandBuffers.push_back(
            _mainCommandPool->allocateCommandBuffer());
    }

    std::vector<std::shared_ptr<ShaderModule>> shaderModules = {
        std::make_shared<ShaderModule>(
            _logicalDevices[0], RESOURCES_DIR "/Shaders/quad_screen.vert"),
        std::make_shared<ShaderModule>(_logicalDevices[0],
                                       RESOURCES_DIR "/Shaders/triangle.frag"),
    };

    std::vector<std::shared_ptr<DescriptorSetLayout>> setLayouts = {};
    std::shared_ptr<PipelineLayout> pipelineLayout =
        std::make_shared<PipelineLayout>(_logicalDevices[0], setLayouts);

    _trianglePipeline = std::make_shared<GraphicsPipeline>(
        _logicalDevices[0], std::move(shaderModules), pipelineLayout);
    _trianglePipeline->initialize(renderPass);
}

RenderDevice::~RenderDevice()
{
    delete _mainCommandPool;
    for (LogicalDevice* logicalDevice : _logicalDevices)
    {
        delete logicalDevice;
    }
    delete _physicalDevice;
    delete _instance;
    delete _deviceSetupCtx;
}

void RenderDevice::beginFrame(const uint32_t deviceIndex)
{
    VOX_ASSERT(deviceIndex < _logicalDevices.size(),
               "Given Index({}), Num LogicalDevices({})", deviceIndex,
               _logicalDevices.size());

    _logicalDevices[deviceIndex]->executeOnEachSwapChain(
        [this](std::shared_ptr<SwapChain> swapChain) {
            const uint32_t currentFrameIndex = swapChain->getFrameIndex();
            swapChain->waitForGpuComplete(currentFrameIndex);

            const auto backBufferIndex =
                swapChain->acquireNextImageIndex().value(); // TODO(snowapril) : handling out-of-date

            CommandBuffer* mainCommandBuffer =
                _mainCommandBuffers[currentFrameIndex].get();

            mainCommandBuffer->beginCommandBuffer(
                swapChain->getSwapChainIndex(), currentFrameIndex,
                backBufferIndex, "MainCommandRendering");

            const std::shared_ptr<Texture> swapChainImage =
                swapChain->getSwapChainImage(backBufferIndex);

            RenderTargetLayoutKey rtLayoutKey = {
                ._debugName = "MainRenderingLayoutKey",
                ._colorAttachmentDescs = { { ._resolution = glm::ivec3(
                                                 swapChain->getResolution(), 1),
                                             ._format =
                                                 swapChain->getSurfaceFormat(),
                                             ._clearColor = true,
                                             ._clearColorValues = glm::vec4(
                                                 0.05f, 0.05f, 0.3f, 1.0f) } },
                ._depthStencilAttachment = std::nullopt,
            };

            RenderTargetsInfo rtInfo = {
                ._debugName = "MainRenderingRTInfo",
                ._layoutKey = rtLayoutKey,
                ._colorRenderTarget = { swapChainImage->getView(0) },
                ._resolution = swapChain->getResolution()
            };

            mainCommandBuffer->setViewport(swapChain->getResolution());
            mainCommandBuffer->beginRenderPass(rtInfo);
            mainCommandBuffer->bindPipeline(_trianglePipeline);
            mainCommandBuffer->endRenderPass();

            mainCommandBuffer->makeSwapChainFinalLayout(swapChain);
            mainCommandBuffer->endCommandBuffer();

            _mainQueue->submitCommandBuffer(
                _mainCommandBuffers[currentFrameIndex], swapChain,
                currentFrameIndex, false);
        });
}

void RenderDevice::presentSwapChains(const uint32_t deviceIndex)
{
    VOX_ASSERT(deviceIndex < _logicalDevices.size(),
               "Given Index({}), Num LogicalDevices({})", deviceIndex,
               _logicalDevices.size());

    _logicalDevices[deviceIndex]->executeOnEachSwapChain(
        [](std::shared_ptr<SwapChain> swapChain) {
            swapChain->present();
            swapChain->prepareForNextFrame();
        });
}

}  // namespace VoxFlow