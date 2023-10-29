// Author : snowapril

#ifndef VOXEL_FLOW_SWAPCHAIN_HPP
#define VOXEL_FLOW_SWAPCHAIN_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <glm/vec2.hpp>
#include <optional>

struct GLFWwindow;

namespace VoxFlow
{
class Instance;
class PhysicalDevice;
class Queue;
class Texture;

// TODO(snowapril) : decouple GLFW dependency from swapchain class
class SwapChain : private NonCopyable
{
 public:
    explicit SwapChain(Instance* instance, PhysicalDevice* physicalDevice, LogicalDevice* logicalDevice, Queue* presentSupportQueue, std::string&& titleName,
                       const glm::uvec2 resolution) noexcept;
    ~SwapChain();
    SwapChain(SwapChain&& other) noexcept;
    SwapChain& operator=(SwapChain&& other) noexcept;

    [[nodiscard]] VkSwapchainKHR get() const noexcept;
    [[nodiscard]] VkSurfaceKHR getSurface() const noexcept;
    [[nodiscard]] VkFormat getSurfaceFormat() const noexcept;
    [[nodiscard]] VkSemaphore getCurrentBackBufferReadySemaphore() const noexcept;
    [[nodiscard]] VkSemaphore getCurrentPresentReadySemaphore() const noexcept;
    [[nodiscard]] uint32_t getSwapChainIndex() const noexcept;
    [[nodiscard]] uint32_t getFrameIndex() const noexcept;
    [[nodiscard]] std::shared_ptr<Texture> getSwapChainImage(const uint32_t index) const;
    [[nodiscard]] glm::uvec2 getResolution() const noexcept;
    [[nodiscard]] GLFWwindow* getGlfwWindow() const noexcept;

 public:
    // Create new SwapChain and resources and discard old one.
    bool create(const bool vsync = false);

    // Release swapchain handle and its resources
    void release();

    // Request usable back buffer from presentation engine
    std::optional<uint32_t> acquireNextImageIndex();

    // Present current back buffer to presentation engine
    void present();

    // Advance frame index for next frame
    void prepareForNextFrame();

    // Add semaphores and waiting values from queue which use swapchain's back
    // buffer to submit command
    void addWaitSemaphores(const uint32_t frameIndex, VkSemaphore timelineSemaphore, const uint64_t waitingValue);

    // Wait all semaphores added with frameIndex
    void waitForGpuComplete(const uint32_t frameIndex);

    // Returns whether swapChain window should be removed.
    bool shouldDestroySwapChain() const;

 private:
    void querySwapChainCapability(VkSwapchainCreateInfoKHR& swapChainCreateInfo, const bool vsync);

 private:
    Instance* _instance = nullptr;
    PhysicalDevice* _physicalDevice = nullptr;
    Queue* _queue = nullptr;
    LogicalDevice* _logicalDevice = nullptr;

    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;

    GLFWwindow* _window = nullptr;
    std::string _titleName;
    glm::uvec2 _resolution;

    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkFormat _surfaceFormat = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR _colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    std::vector<std::shared_ptr<Texture>> _swapChainImages;

    std::vector<VkSemaphore> _backBufferReadySemaphores;
    std::vector<VkSemaphore> _presentSemaphores;

    uint32_t _swapChainIndex = 0;
    uint32_t _backBufferIndex = 0;
    uint32_t _frameIndex = 0;

 private:
    struct SemaphoreWaitInfo
    {
        std::mutex _waitInfoMutex;
        std::vector<VkSemaphore> _waitSemaphores;
        std::vector<uint64_t> _waitingSemaphoreValues;
    };
    std::array<SemaphoreWaitInfo, FRAME_BUFFER_COUNT> _waitSemaphoreInfos;
};

inline VkSwapchainKHR SwapChain::get() const noexcept
{
    return _swapChain;
}
inline VkSurfaceKHR SwapChain::getSurface() const noexcept
{
    return _surface;
}
inline VkFormat SwapChain::getSurfaceFormat() const noexcept
{
    return _surfaceFormat;
}
inline VkSemaphore SwapChain::getCurrentBackBufferReadySemaphore() const noexcept
{
    return _backBufferReadySemaphores[_frameIndex];
}
inline VkSemaphore SwapChain::getCurrentPresentReadySemaphore() const noexcept
{
    return _presentSemaphores[_frameIndex];
}
inline uint32_t SwapChain::getSwapChainIndex() const noexcept
{
    return _swapChainIndex;
}
inline uint32_t SwapChain::getFrameIndex() const noexcept
{
    return _frameIndex;
}
inline std::shared_ptr<Texture> SwapChain::getSwapChainImage(const uint32_t index) const
{
    VOX_ASSERT(index < static_cast<uint32_t>(_swapChainImages.size()), "Given Index ({}), Num SwapChain Images ({})", index, _swapChainImages.size());
    return _swapChainImages[index];
}
inline glm::uvec2 SwapChain::getResolution() const noexcept
{
    return _resolution;
}
inline GLFWwindow* SwapChain::getGlfwWindow() const noexcept
{
    return _window;
}

}  // namespace VoxFlow

#endif