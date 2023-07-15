// Author : snowapril

#include <VoxFlow/Core/Utils/DeviceInputSubscriber.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <vector>
#include <GLFW/glfw3.h>

namespace
{
std::vector<GLFWwindow*> gGlfwWindows;
VoxFlow::DeviceInputSubscriber* gDeviceInputRegistrator = nullptr;
}  // namespace

void deviceKeyInputCallback(GLFWwindow* window, int key, int scancode,
                            int action, int mod)
{
    using namespace VoxFlow;

    (void)mod;
    (void)scancode;

    DeviceKeyInputType keyInputType = DeviceKeyInputType::Undefined;
    bool isReleased = true;

    switch (key)
    {
        case GLFW_KEY_ESCAPE:
            keyInputType = DeviceKeyInputType::Escape;
            break;
    }

    switch (action)
    {
        case GLFW_PRESS:
            isReleased = false;
            break;
        case GLFW_RELEASE:
            isReleased = true;
        default:
            break;
    }

    if (gDeviceInputRegistrator != nullptr)
    {
        const auto iter = std::find(gGlfwWindows.begin(), gGlfwWindows.end(), window);
        VOX_ASSERT(iter != gGlfwWindows.end(),
                   "Iternal logic error releated to device input registrator");

        const auto windowIndex =
            static_cast<uint32_t>(std::distance(gGlfwWindows.begin(), iter));

        gDeviceInputRegistrator->broadcastKeyInput(windowIndex, keyInputType, isReleased);
    }
}

namespace VoxFlow
{

DeviceInputSubscriber::DeviceInputSubscriber()
{
    gDeviceInputRegistrator = this;
}

DeviceInputSubscriber::~DeviceInputSubscriber()
{
    gGlfwWindows.clear();
    gDeviceInputRegistrator = nullptr;
}

void DeviceInputSubscriber::addObserveTargetWindow(GLFWwindow* window)
{
    gGlfwWindows.push_back(window);

    glfwSetKeyCallback(window, deviceKeyInputCallback);
}

void DeviceInputSubscriber::removeObserveTargetWindow(GLFWwindow* window)
{
    auto iter = std::find(gGlfwWindows.begin(), gGlfwWindows.end(),
                  window);

    if (iter != gGlfwWindows.end())
    {
        gGlfwWindows.erase(iter);
    }
    else
    {
        VOX_ASSERT(false, "This GLFWwindow({}) has never been registered",
                   fmt::ptr(window));
    }
}

void DeviceInputSubscriber::registerDeviceKeyCallback(
    const uint32_t swapChainBits, DeviceKeyInputCallback&& callback)
{
    _deviceKeyInputCallbacks.emplace_back(swapChainBits, std::move(callback));
}

void DeviceInputSubscriber::broadcastKeyInput(uint32_t windowIndex,
                                               DeviceKeyInputType key,
                                               bool isReleased)
{
    for (const auto& callback : _deviceKeyInputCallbacks)
    {
        if (callback.first.test(windowIndex))
        {
            std::invoke(callback.second, key, isReleased);
        }
    }
}
}  // namespace VoxFlow