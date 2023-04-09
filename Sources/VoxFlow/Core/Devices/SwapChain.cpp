// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>

#include <GLFW/glfw3.h>
#include <glm/common.hpp>

namespace VoxFlow
{
SwapChain::SwapChain(Instance* instance, PhysicalDevice* physicalDevice,
                     LogicalDevice* logicalDevice, Queue* presentSupportQueue,
                     std::string&& titleName, const glm::uvec2 resolution) noexcept
    : _instance(instance),
      _physicalDevice(physicalDevice),
      _queue(presentSupportQueue),
      _logicalDevice(logicalDevice),
      _titleName(std::move(titleName)),
      _resolution(resolution)
{
    // TODO(snowapril) : check glfwGetPhysicalDevicePresentationSupport
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window = glfwCreateWindow(resolution.x, resolution.y, titleName.c_str(),
                               nullptr,
                               nullptr);

    // TODO(snowapril) : move callback registration other place
    glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode,
                                   int action, int mods) {
        (void)scancode;
        (void)mods;
        if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
            glfwSetWindowShouldClose(window, GLFW_TRUE);
    });

    VK_ASSERT(
        glfwCreateWindowSurface(_instance->get(), _window, nullptr, &_surface));

    const std::vector<VkSurfaceFormatKHR> surfaceFormatList = {
        { VK_FORMAT_R8G8B8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR },
        { VK_FORMAT_B8G8R8A8_SRGB, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR }
    };

    uint32_t numFormats{ 0 };
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        _physicalDevice->get(), _surface, &numFormats, nullptr));

    std::vector<VkSurfaceFormatKHR> deviceSurfaceFormats(numFormats);
    VK_ASSERT(vkGetPhysicalDeviceSurfaceFormatsKHR(
        _physicalDevice->get(), _surface, &numFormats,
        deviceSurfaceFormats.data()));

    for (const VkSurfaceFormatKHR& deviceSurfaceFormat : deviceSurfaceFormats)
    {
        for (const VkSurfaceFormatKHR& preferredSurfaceFormat :
             surfaceFormatList)
        {
            if ((deviceSurfaceFormat.format == preferredSurfaceFormat.format) &&
                (deviceSurfaceFormat.colorSpace ==
                 preferredSurfaceFormat.colorSpace))
            {
                _surfaceFormat = deviceSurfaceFormat.format;
                _colorSpace = deviceSurfaceFormat.colorSpace;
                break;
            }
        }
    }
}

SwapChain::~SwapChain()
{
    release();
}

SwapChain::SwapChain(SwapChain&& other) noexcept
{
    operator=(std::move(other));
}

SwapChain& SwapChain::operator=(SwapChain&& other) noexcept
{
    if (this != &other)
    {
        _swapChain = other._swapChain;
        _surface = other._surface;
        _window = other._window;
        _instance = other._instance;
        _physicalDevice = other._physicalDevice;
        _logicalDevice = other._logicalDevice;
        _queue = other._queue;
        _resolution = other._resolution;
        _surfaceFormat = other._surfaceFormat;
        _colorSpace = other._colorSpace;
        _swapChainImages.swap(other._swapChainImages);
    }
    return *this;
}

std::optional<uint32_t> SwapChain::acquireNextImageIndex()
{
    VK_ASSERT(vkAcquireNextImageKHR(_logicalDevice->get(), _swapChain,
                                    UINT64_MAX,
                                    _backBufferReadySemaphores[_frameIndex],
                                    VK_NULL_HANDLE, &_backBufferIndex));

    // TODO(snowapril) : need handling out-of-date or not-ready situation.
    return { _backBufferIndex };
}

bool SwapChain::create(const bool vsync)
{
    VkSwapchainKHR oldSwapChain = _swapChain;

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {};
    querySwapChainCapability(swapChainCreateInfo, vsync);
    swapChainCreateInfo.oldSwapchain = oldSwapChain;

    VK_ASSERT(vkCreateSwapchainKHR(_logicalDevice->get(), &swapChainCreateInfo,
                                   nullptr, &_swapChain));

    if (_swapChain == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to create SwapChain({})", _titleName);
        return false;
    }

    DebugUtil::setObjectName(_logicalDevice, _swapChain, _titleName.c_str());

    if (oldSwapChain != VK_NULL_HANDLE)
    {
        _swapChainImages.clear();
        vkDestroySwapchainKHR(_logicalDevice->get(), oldSwapChain, nullptr);
    }

    uint32_t numSwapChainImages;
    vkGetSwapchainImagesKHR(_logicalDevice->get(), _swapChain,
                            &numSwapChainImages, nullptr);
    
    std::vector<VkImage> vkSwapChainImages(numSwapChainImages);
    vkGetSwapchainImagesKHR(_logicalDevice->get(), _swapChain,
                            &numSwapChainImages, vkSwapChainImages.data());

    // Create Textures from queried swapChain images
    _swapChainImages.reserve(numSwapChainImages);
    for (uint32_t i = 0; i < numSwapChainImages; ++i)
    {
        auto swapChainImage = std::make_shared<Texture>(
            fmt::format("{}_Image({})", _titleName, i), _logicalDevice,
            nullptr);

        TextureInfo surfaceInfo = { ._extent = glm::uvec3(_resolution, 1),
                                    ._format = _surfaceFormat,
                                    ._imageType = VK_IMAGE_TYPE_2D,
                                    ._usage = TextureUsage::RenderTarget };

        swapChainImage->initializeFromSwapChain(surfaceInfo,
                                                vkSwapChainImages[i]);

        TextureViewInfo surfaceViewInfo = { ._viewType = VK_IMAGE_VIEW_TYPE_2D,
                                            ._format = _surfaceFormat,
                                            ._aspectFlags =
                                                VK_IMAGE_ASPECT_COLOR_BIT,
                                            ._baseMipLevel = 0,
                                            ._levelCount = 1,
                                            ._baseArrayLayer = 0,
                                            ._layerCount = 1 };

        swapChainImage->createTextureView(surfaceViewInfo);

        _swapChainImages.push_back(std::move(swapChainImage));
    }

    // If number of swapChain images are differred with original, create new semaphore
    if (numSwapChainImages != _presentSemaphores.size())
    {
        LogicalDevice* logicalDevice = _logicalDevice;
        auto recreateSemaphores =
            [logicalDevice](const uint32_t numSwapChainImages,
                            std::vector<VkSemaphore>& targetSemaphores) {
                for (uint32_t i = numSwapChainImages;
                     i < targetSemaphores.size(); ++i)
                {
                    vkDestroySemaphore(logicalDevice->get(),
                                       targetSemaphores[i], nullptr);
                }

                const uint32_t oldNumSemaphores =
                    static_cast<uint32_t>(targetSemaphores.size());
                targetSemaphores.resize(numSwapChainImages);
                for (uint32_t i = oldNumSemaphores; i < numSwapChainImages; ++i)
                {
                    VkSemaphoreCreateInfo semaphoreInfo =
                        Initializer::MakeInfo<VkSemaphoreCreateInfo>();
                    vkCreateSemaphore(logicalDevice->get(), &semaphoreInfo,
                                      nullptr, &targetSemaphores[i]);
                }
            };

        recreateSemaphores(numSwapChainImages, _backBufferReadySemaphores);
        recreateSemaphores(numSwapChainImages, _presentSemaphores);
    }

    return true;
}

void SwapChain::querySwapChainCapability(
    VkSwapchainCreateInfoKHR& swapChainCreateInfo, const bool vsync)
{
    VkSurfaceCapabilitiesKHR surfaceCaps = {};
    VK_ASSERT(vkGetPhysicalDeviceSurfaceCapabilitiesKHR(
        _physicalDevice->get(), _surface, &surfaceCaps));

    uint32_t numPresentModes = 0;
    vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice->get(), _surface,
                                              &numPresentModes, nullptr);

    std::vector<VkPresentModeKHR> supportedPresentModes(numPresentModes);
    vkGetPhysicalDeviceSurfacePresentModesKHR(_physicalDevice->get(), _surface,
                                              &numPresentModes,
                                              supportedPresentModes.data());

    if (surfaceCaps.currentExtent.width == UINT32_MAX ||
        surfaceCaps.currentExtent.height == UINT32_MAX)
    {
        surfaceCaps.currentExtent.width = _resolution.x;
        surfaceCaps.currentExtent.height = _resolution.y;
    }
    else
    {
        _resolution.x = surfaceCaps.currentExtent.width;
        _resolution.y = surfaceCaps.currentExtent.height;
    }

    const VkExtent2D swapchainExtent{
        .width = static_cast<uint32_t>(_resolution.x),
        .height = static_cast<uint32_t>(_resolution.y)
    };

    VkPresentModeKHR resultPresentMode = VK_PRESENT_MODE_FIFO_KHR;
    if (vsync == false)
    {
        for (const VkPresentModeKHR& presentMode : supportedPresentModes)
        {
            if (presentMode == VK_PRESENT_MODE_MAILBOX_KHR)
            {
                resultPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
                break;
            }
            else if (presentMode == VK_PRESENT_MODE_IMMEDIATE_KHR)
            {
                resultPresentMode = VK_PRESENT_MODE_IMMEDIATE_KHR;
                break;
            }
        }
    }

    uint32_t numDesiredSwapChainImages = surfaceCaps.minImageCount + 1;
    if ((surfaceCaps.maxImageCount > 0))
    {
        numDesiredSwapChainImages =
            glm::min(numDesiredSwapChainImages, surfaceCaps.maxImageCount);
    }

    VkSurfaceTransformFlagBitsKHR preTransform;
    if (surfaceCaps.supportedTransforms ==
        VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR)
    {
        preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    }
    else
    {
        preTransform = surfaceCaps.currentTransform;
    }

    VkCompositeAlphaFlagBitsKHR compositeAlpha =
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;

    VkCompositeAlphaFlagBitsKHR compositeAlphaList[] = {
        VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR,
        VK_COMPOSITE_ALPHA_PRE_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_POST_MULTIPLIED_BIT_KHR,
        VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR,
    };

    for (VkCompositeAlphaFlagBitsKHR alphaFlagBit : compositeAlphaList)
    {
        if (surfaceCaps.supportedCompositeAlpha & alphaFlagBit)
        {
            compositeAlpha = alphaFlagBit;
            break;
        }
    }

    swapChainCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR,
        .pNext = nullptr,
        .flags = 0,
        .surface = _surface,
        .minImageCount = numDesiredSwapChainImages,
        .imageFormat = _surfaceFormat,
        .imageColorSpace = _colorSpace,
        .imageExtent = swapchainExtent,
        .imageArrayLayers = 1,
        .imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
        .imageSharingMode = VK_SHARING_MODE_EXCLUSIVE,
        .queueFamilyIndexCount = 0,
        .pQueueFamilyIndices = nullptr,
        .preTransform = preTransform,
        .compositeAlpha = compositeAlpha,
        .presentMode = resultPresentMode,
        .clipped = VK_TRUE,  // Allow presentation engine discard rendering
                             // outside of the surface,
        .oldSwapchain = VK_NULL_HANDLE
    };

    if (surfaceCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if (surfaceCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }
}

void SwapChain::release()
{
    for (VkSemaphore& backBufferReadySemaphore : _backBufferReadySemaphores)
    {
        vkDestroySemaphore(_logicalDevice->get(), backBufferReadySemaphore,
                           nullptr);
    }
    _backBufferReadySemaphores.clear();

    for (VkSemaphore& presentSemaphore : _presentSemaphores)
    {
        vkDestroySemaphore(_logicalDevice->get(), presentSemaphore, nullptr);
    }
    _presentSemaphores.clear();

    if (_swapChain != VK_NULL_HANDLE)
    {
        vkDestroySwapchainKHR(_logicalDevice->get(), _swapChain, nullptr);
    }

    if (_surface != VK_NULL_HANDLE)
    {
        vkDestroySurfaceKHR(_instance->get(), _surface, nullptr);
    }

    if (_window != nullptr)
    {
        glfwDestroyWindow(_window);
    }
}

void SwapChain::present()
{
    VkResult presentResult = VK_SUCCESS;

    VkPresentInfoKHR presentInfo = {
        .sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR,
        .pNext = nullptr,
        .waitSemaphoreCount = 1,
        .pWaitSemaphores = &_presentSemaphores[_frameIndex],
        .swapchainCount = 1,
        .pSwapchains = &_swapChain,
        .pImageIndices = &_backBufferIndex,
        .pResults = &presentResult,
    };

    VK_ASSERT(vkQueuePresentKHR(_queue->get(), &presentInfo));
}

void SwapChain::prepareForNextFrame()
{
    _frameIndex = (_frameIndex + 1) % FRAME_BUFFER_COUNT;
}

void SwapChain::addWaitSemaphores(const uint32_t frameIndex,
    VkSemaphore timelineSemaphore,
    const uint64_t waitingValue)
{
    SemaphoreWaitInfo& waitInfo =
        _waitSemaphoreInfos[frameIndex];

    std::lock_guard<std::mutex> scopeLockGuard(waitInfo._waitInfoMutex);

    waitInfo._waitSemaphores.push_back(timelineSemaphore);
    waitInfo._waitingSemaphoreValues.push_back(
        waitingValue);
}

void SwapChain::waitForGpuComplete(const uint32_t frameIndex)
{
    SemaphoreWaitInfo& waitInfo = _waitSemaphoreInfos[frameIndex];

    std::lock_guard<std::mutex> scopeLockGuard(waitInfo._waitInfoMutex);

    if (waitInfo._waitSemaphores.empty() == false)
    {
        VkSemaphoreWaitInfo vkWaitInfo = {
            .sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO_KHR,
            .pNext = nullptr,
            .flags = 0,
            .semaphoreCount =
                static_cast<uint32_t>(waitInfo._waitSemaphores.size()),
            .pSemaphores = waitInfo._waitSemaphores.data(),
            .pValues = waitInfo._waitingSemaphoreValues.data()
        };
        VK_ASSERT(vkWaitSemaphoresKHR(_logicalDevice->get(), &vkWaitInfo,
                                      UINT64_MAX));

        waitInfo._waitSemaphores.clear();
        waitInfo._waitingSemaphoreValues.clear();
    }
}

bool SwapChain::shouldDestroySwapChain() const
{
    return glfwWindowShouldClose(_window);
}
}  // namespace VoxFlow