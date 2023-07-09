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
#include <VoxFlow/Core/Renderer/SceneRenderer.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp>
#include <VoxFlow/Core/Devices/RenderDevice.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Resources/RenderResourceGarbageCollector.hpp>

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
    deviceSetupCtx.addDeviceExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);
    deviceSetupCtx.addDeviceExtension(
        VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME);

    _deviceSetupCtx = new Context(deviceSetupCtx);
    _instance = new Instance(deviceSetupCtx);
    _physicalDevice = new PhysicalDevice(_instance);

    // TODO(snowapril) : support multiple logical devices
    _logicalDevices.emplace_back(
        std::make_unique<LogicalDevice>(deviceSetupCtx, _physicalDevice, _instance));
    _logicalDevices[0]->addSwapChain("VoxFlow Editor", glm::ivec2(1280, 920));

    _sceneRenderer =
        std::make_unique<SceneRenderer>(_logicalDevices[0].get(), &_frameGraph);

    RenderResourceGarbageCollector::Get().threadConstruct();
}

RenderDevice::~RenderDevice()
{
    for (std::unique_ptr<LogicalDevice>& logicalDevice : _logicalDevices)
    {
        logicalDevice->releaseDedicatedResources();
    }
    
    RenderResourceGarbageCollector::Get().threadTerminate();
    _logicalDevices.clear();

    delete _physicalDevice;
    delete _instance;
    delete _deviceSetupCtx;
}

}  // namespace VoxFlow