// Author : snowapril

#include <chrono>
#include <VoxFlow/Editor/VoxEditor.hpp>
#include <VoxFlow/Core/RenderDevice.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <GLFW/glfw3.h>
#include <VoxFlow/Editor/VoxFlow.hpp>

namespace VoxFlow
{

VoxEditor::VoxEditor()
{
    if (glfwInit() == GLFW_FALSE)
    {
        const char* glfwErrorMsg = nullptr;
        glfwGetError(&glfwErrorMsg);
        VOX_ASSERT(false, "Failed to initialize GLWF. LastError : {}",
                   glfwErrorMsg);
        return;
    }

    glfwSetErrorCallback(DebugUtil::GlfwDebugCallback);

    if (glfwVulkanSupported() == GLFW_FALSE)
    {
        const char* glfwErrorMsg = nullptr;
        glfwGetError(&glfwErrorMsg);
        VOX_ASSERT(false,
                   "Current availiability of loader and ICD does not support "
                   "Vulkan. LastError : {}",
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
    context.addDeviceExtension(VK_KHR_TIMELINE_SEMAPHORE_EXTENSION_NAME);

    _renderDevice = new RenderDevice(context);

    _voxFlow = new VoxFlow();
    _voxFlow->initialize();
}

VoxEditor::~VoxEditor()
{
    if (_renderDevice != nullptr)
    {
        delete _renderDevice;
    }

    glfwTerminate();
}

void VoxEditor::runEditorLoop()
{
    using namespace std::chrono;
    bool exit = false;

    // system_clock::time_point previousTime = system_clock::now();

    while (exit == false)
    {
        // system_clock::time_point currentTime = system_clock::now();
        // const uint64_t elapsed =
        //     duration_cast<milliseconds>(currentTime - previousTime).count();
        // previousTime = currentTime;

        processInput();

        preUpdateFrame();
        updateFrame();
        renderFrame();
        postRenderFrame();
    }
}

void VoxEditor::processInput()
{
    glfwPollEvents();
}

void VoxEditor::preUpdateFrame()
{
}

void VoxEditor::updateFrame()
{
}

void VoxEditor::renderFrame()
{
    // TODO(snowapril) : customize logical device index
    _renderDevice->beginFrame(0);
    _renderDevice->presentSwapChains(0);
}

void VoxEditor::postRenderFrame()
{
}

}  // namespace VoxFlow