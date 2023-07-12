// Author : snowapril

#include <chrono>
#include <VoxFlow/Editor/VoxEditor.hpp>
#include <VoxFlow/Editor/RenderPass/SceneObjectPass.hpp>
#include <VoxFlow/Editor/RenderPass/PostProcessPass.hpp>
#include <VoxFlow/Core/Devices/RenderDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderer.hpp>
#include <GLFW/glfw3.h>

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

    _renderDevice = new RenderDevice(context);

    _inputRegistrator.addObserveTargetWindow(
        _renderDevice->getLogicalDevice(0)->getSwapChain(0)->getGlfwWindow());

    using namespace std::placeholders;
    auto processKeyCallback = std::mem_fn(&VoxEditor::processKeyInput);
    _inputRegistrator.registerDeviceKeyCallback(
        uint32_t(-1), std::bind(processKeyCallback, this, _1, _2));

    SceneRenderer* sceneRenderer = _renderDevice->getSceneRenderer();
    _sceneObjectPass =
        sceneRenderer->getOrCreateSceneRenderPass<SceneObjectPass>(
            "SceneObjectPass");
    _postProcessPass =
        sceneRenderer->getOrCreateSceneRenderPass<PostProcessPass>(
            "PostProcessPass");
    _postProcessPass->addDependency("SceneObjectPass");
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
    auto previousTime = std::chrono::system_clock::now();

    while (_shouldCloseEditor == false)
    {
        const auto currentTime = std::chrono::system_clock::now();
        const double elapsed =
            std::chrono::duration<double>(currentTime - previousTime).count();
        previousTime = currentTime;

        glfwPollEvents();

        _renderDevice->updateRender(elapsed);

        _renderDevice->renderScene();
    }
}

void VoxEditor::processKeyInput(DeviceKeyInputType key, const bool isReleased)
{
    if (isReleased == false)
    {
        switch (key)
        {
            case DeviceKeyInputType::Escape:
                _shouldCloseEditor = true;
                break;
        }
    }
}

}  // namespace VoxFlow