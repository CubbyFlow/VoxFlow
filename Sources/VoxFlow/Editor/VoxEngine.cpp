// Author : snowapril

#include <VoxFlow/Editor/VoxEngine.hpp>
#include <VoxFlow/Core/RenderDevice.hpp>

namespace VoxFlow
{

VoxEngine::VoxEngine()
{
    Context context;
    context.setVersion(1, 3);
    context.addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    context.addRequiredQueue(
        "GCT",
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1,
        1.0F, true);

    _renderDevice = new RenderDevice(context);
}

VoxEngine::~VoxEngine()
{
    if (_renderDevice != nullptr)
    {
        delete _renderDevice;
    }
}

}  // namespace VoxFlow