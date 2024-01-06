// Author : snowapril

#include <GLFW/glfw3.h>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/RenderDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderer.hpp>
#include <VoxFlow/Core/Utils/ChromeTracer.hpp>
#include <VoxFlow/Editor/RenderPass/PostProcessPass.hpp>
#include <VoxFlow/Editor/RenderPass/SceneObjectPass.hpp>
#include <VoxFlow/Editor/VoxEditor.hpp>
#include <chrono>

namespace VoxFlow
{

VoxEditor::VoxEditor(cxxopts::ParseResult&& arguments)
{
    if (glfwInit() == GLFW_FALSE)
    {
        const char* glfwErrorMsg = nullptr;
        glfwGetError(&glfwErrorMsg);
        VOX_ASSERT(false, "Failed to initialize GLFW. LastError : {}", glfwErrorMsg);
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

    const bool enableDebugLayer = arguments["debug"].as<bool>();

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

    LogicalDevice* mainLogicalDevice = _renderDevice->getLogicalDevice(LogicalDeviceType::MainDevice);

    _inputRegistrator.addObserveTargetWindow(mainLogicalDevice->getSwapChain(0)->getGlfwWindow());

    using namespace std::placeholders;
    auto processKeyCallback = std::mem_fn(&VoxEditor::processKeyInput);
    _inputRegistrator.registerDeviceKeyCallback(uint32_t(-1), std::bind(processKeyCallback, this, _1, _2));

    SceneRenderer* sceneRenderer = _renderDevice->getSceneRenderer();
    _sceneObjectPass = sceneRenderer->getOrCreateSceneRenderPass<SceneObjectPass>("SceneObjectPass", mainLogicalDevice);
    _postProcessPass = sceneRenderer->getOrCreateSceneRenderPass<PostProcessPass>("PostProcessPass", mainLogicalDevice);
    _postProcessPass->addDependency("SceneObjectPass");

    sceneRenderer->initializePasses();
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
        const double elapsed = std::chrono::duration<double>(currentTime - previousTime).count();
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
            case DeviceKeyInputType::KeyC:
                if (HAS_TRACING_BEGIN())
                {
                    END_CHROME_TRACING("editor_tracing.json");
                }
                else
                {
                    BEGIN_CHROME_TRACING();
                }
                break;
            default:
                break;
        }
    }
}

}  // namespace VoxFlow