// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Devices/Swapchain.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

#include <glfw/glfw3.h>
#include <glm/common.hpp>

namespace VoxFlow
{
SwapChain::SwapChain(Instance* instance, PhysicalDevice* physicalDevice,
                     LogicalDevice* logicalDevice, Queue* presentSupportQueue,
                     const char* title, const glm::ivec2 resolution) noexcept
    : _instance(instance),
      _physicalDevice(physicalDevice),
      _logicalDevice(logicalDevice),
      _queue(presentSupportQueue),
      _resolution(resolution)
{
    // TODO(snowapril) : check glfwGetPhysicalDevicePresentationSupport 

    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
    _window =
        glfwCreateWindow(resolution.x, resolution.y, title, nullptr, nullptr);

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
        _swapChainImageViews.swap(other._swapChainImageViews);
    }
    return *this;
}

bool SwapChain::create(bool vsync)
{
    VkSwapchainKHR oldSwapChain = _swapChain;

    VkSurfaceCapabilitiesKHR surfaceCaps;
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

    VkExtent2D swapchainExtent(_resolution.x, _resolution.y);

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
                resultPresentMode = VK_PRESENT_MODE_MAILBOX_KHR;
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

    VkSwapchainCreateInfoKHR swapChainCreateInfo = {
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
        .oldSwapchain = oldSwapChain
    };

    if (surfaceCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_SRC_BIT)
    {
        swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
    }

    if (surfaceCaps.supportedUsageFlags & VK_IMAGE_USAGE_TRANSFER_DST_BIT)
    {
        swapChainCreateInfo.imageUsage |= VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    }

    VK_ASSERT(vkCreateSwapchainKHR(_logicalDevice->get(), &swapChainCreateInfo,
                                   nullptr, &_swapChain));

    if (oldSwapChain != VK_NULL_HANDLE)
    {
        for (VkImageView& swapChainImageView : _swapChainImageViews)
        {
            vkDestroyImageView(_logicalDevice->get(), swapChainImageView,
                               nullptr);
        }
        _swapChainImageViews.clear();

        vkDestroySwapchainKHR(_logicalDevice->get(), oldSwapChain, nullptr);
    }

    uint32_t numSwapChainImages;
    vkGetSwapchainImagesKHR(_logicalDevice->get(), _swapChain,
                            &numSwapChainImages, nullptr);
    _swapChainImages.resize(numSwapChainImages);
    _swapChainImageViews.resize(numSwapChainImages);
    vkGetSwapchainImagesKHR(_logicalDevice->get(), _swapChain,
                            &numSwapChainImages, _swapChainImages.data());

    for (uint32_t i = 0; i < numSwapChainImages; ++i)
    {
        VkImageViewCreateInfo viewCreateInfo = {
            .sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
            .pNext = nullptr,
            .flags = 0,
             .image = _swapChainImages[i],
            .viewType = VK_IMAGE_VIEW_TYPE_2D,
            .format = _surfaceFormat,
            .components = {
                VK_COMPONENT_SWIZZLE_R,
                VK_COMPONENT_SWIZZLE_G,
                VK_COMPONENT_SWIZZLE_B,
                VK_COMPONENT_SWIZZLE_A,
            },
            .subresourceRange = {
                .aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
                .baseMipLevel = 0,
                .levelCount = 1,
                .baseArrayLayer = 0,
                .layerCount = 1
            }
        };

        VK_ASSERT(vkCreateImageView(_logicalDevice->get(), &viewCreateInfo,
                                    nullptr, &_swapChainImageViews[i]));
    }

    return true;
}

void SwapChain::release()
{
    if (_swapChain != VK_NULL_HANDLE)
    {
        for (VkImageView& swapChainImageView : _swapChainImageViews)
        {
            vkDestroyImageView(_logicalDevice->get(), swapChainImageView,
                               nullptr);
        }
        _swapChainImageViews.clear();

        vkDestroySwapchainKHR(_logicalDevice->get(), _swapChain, nullptr);
    }

    vkDestroySurfaceKHR(_instance->get(), _surface, nullptr);
    glfwDestroyWindow(_window);
}
}  // namespace VoxFlow