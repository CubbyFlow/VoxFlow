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

class SwapChain : NonCopyable
{
 public:
    explicit SwapChain(Instance* instance, PhysicalDevice* physicalDevice,
                       LogicalDevice* logicalDevice, Queue* presentSupportQueue,
                       std::string&& titleName, const glm::ivec2 resolution) noexcept;
    ~SwapChain();
    SwapChain(SwapChain&& other) noexcept;
    SwapChain& operator=(SwapChain&& other) noexcept;

    [[nodiscard]] inline VkSwapchainKHR get() const noexcept
    {
        return _swapChain;
    }

    [[nodiscard]] inline VkSurfaceKHR getSurface() const noexcept
    {
        return _surface;
    }

    [[nodiscard]] inline VkSemaphore getCurrentBackBufferReadySemaphore() const
    {
        return _backBufferReadySemaphores[_frameIndex];
    }
    [[nodiscard]] inline VkSemaphore getCurrentPresentReadySemaphore() const
    {
        return _presentSemaphores[_frameIndex];
    }

    [[nodiscard]] inline uint32_t getSwapChainIndex() const
    {
        return _swapChainIndex;
    }

    [[nodiscard]] inline uint32_t getFrameIndex() const
    {
        return _frameIndex;
    }

    [[nodiscard]] VkImage getSwapChainImage(const uint32_t index) const
    {
        VOX_ASSERT(index < static_cast<uint32_t>(_swapChainImages.size()),
                   "Given Index (%u), Num SwapChain Images (%u)", index,
                   _swapChainImages.size());
        return _swapChainImages[index];
    }

    public:
    // Create new SwapChain and resources and discard old one.
    bool create(bool vsync = false);

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
    void addWaitSemaphores(const uint32_t frameIndex,
                           VkSemaphore timelineSemaphore,
                           const uint64_t waitingValue);

    // Wait all semaphores added with frameIndex
    void waitForGpuComplete(const uint32_t frameIndex);

 private:
    Instance* _instance = nullptr;
    PhysicalDevice* _physicalDevice = nullptr;
    Queue* _queue = nullptr;
    LogicalDevice* _logicalDevice = nullptr;

    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;

    GLFWwindow* _window = nullptr;
    std::string _titleName;
    glm::ivec2 _resolution;

    VkSurfaceKHR _surface = VK_NULL_HANDLE;
    VkFormat _surfaceFormat = VK_FORMAT_UNDEFINED;
    VkColorSpaceKHR _colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR;
    std::vector<VkImage> _swapChainImages;
    std::vector<VkImageView> _swapChainImageViews;

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
}  // namespace VoxFlow

#endif