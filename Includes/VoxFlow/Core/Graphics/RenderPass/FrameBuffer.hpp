// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_BUFFER_HPP
#define VOXEL_FLOW_FRAME_BUFFER_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderTargetGroup.hpp>

namespace VoxFlow
{
class LogicalDevice;
class RenderPass;

class FrameBuffer : private NonCopyable
{
 public:
    explicit FrameBuffer(LogicalDevice* logicalDevice);
    ~FrameBuffer() override;
    FrameBuffer(FrameBuffer&& other) noexcept;
    FrameBuffer& operator=(FrameBuffer&& other) noexcept;

    [[nodiscard]] inline VkFramebuffer get() const noexcept
    {
        return _vkFrameBuffer;
    }

    [[nodiscard]] inline const RenderTargetsInfo& getRenderTargetsInfo() const noexcept
    {
        return _renderTargetsInfo;
    }

 public:
    bool initialize(const RenderTargetsInfo& rtInfo);
    void release();

 private:
    LogicalDevice* _logicalDevice = nullptr;
    RenderTargetsInfo _renderTargetsInfo;
    VkFramebuffer _vkFrameBuffer = VK_NULL_HANDLE;
};
}  // namespace VoxFlow

#endif