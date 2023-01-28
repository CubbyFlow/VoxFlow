// Author : snowapril

#ifndef VOXEL_FLOW_SWAPCHAIN_HPP
#define VOXEL_FLOW_SWAPCHAIN_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class SwapChain : NonCopyable
{
 public:
    explicit SwapChain() noexcept;
    ~SwapChain() override = default;
    SwapChain(SwapChain&& other) noexcept;
    SwapChain& operator=(SwapChain&& other) noexcept;

    [[nodiscard]] inline VkSwapchainKHR get() const noexcept
    {
        return _swapChain;
    }

 private:
    VkSwapchainKHR _swapChain = VK_NULL_HANDLE;
};
}  // namespace VoxFlow

#endif