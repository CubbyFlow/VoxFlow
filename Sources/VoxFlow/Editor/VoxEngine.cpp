// Author : snowapril

#include <VoxFlow/Core/RenderDevice.hpp>
#include <VoxFlow/Editor/VoxEngine.hpp>
#include <glfw/glfw3.h>

namespace VoxFlow
{

VoxEngine::VoxEngine()
{
    if (glfwInit() == GLFW_FALSE)
    {
        const char* glfwErrorMsg = nullptr;
        glfwGetError(&glfwErrorMsg);
        VOX_ASSERT(false, "Failed to initialize GLWF. LastError : %s",
                   glfwErrorMsg);
        return;
    }

    if (glfwVulkanSupported() == GLFW_FALSE)
    {
        const char* glfwErrorMsg = nullptr;
        glfwGetError(&glfwErrorMsg);
        VOX_ASSERT(false, "Current availiability of loader and ICD does not support Vulkan. LastError : %s",
                   glfwErrorMsg);
        return;
    }

    constexpr bool enableDebugLayer = true;

    Context context(enableDebugLayer);
    context.setVersion(1, 3);
    
    // Add GLFW required instace extensions and swapchain device extension
    {
        uint32_t numGlfwExtensions = 0;
        const char** glfwExtensions = nullptr;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&numGlfwExtensions);
        for (uint32_t i = 0; i < numGlfwExtensions; ++i)
        {
            context.addInstanceExtension(glfwExtensions[i]);
        }
        context.addDeviceExtension(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    }

    // TODO(snowapril) : customize
    context.addRequiredQueue(
        "GCT",
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1,
        1.0F, true);

    _renderDevice = new RenderDevice(context);

    // TODO(snowapril) : resolution desc, ...
    _renderDevice->addSwapChain("VoxFlow Editor", glm::ivec2(1280, 920));
}

VoxEngine::~VoxEngine()
{
    if (_renderDevice != nullptr)
    {
        delete _renderDevice;
    }

    glfwTerminate();
}

}  // namespace VoxFlow