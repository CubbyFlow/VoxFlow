// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_BUFFER_HPP
#define VOXEL_FLOW_COMMAND_BUFFER_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <string>

namespace VoxFlow
{
class Queue;
class CommandPool;
class RenderPass;
class SwapChain;
class BasePipeline;

class CommandBuffer : private NonCopyable
{
 public:
    explicit CommandBuffer(Queue* commandQueue, CommandPool* commandPool, VkCommandBuffer vkCommandBuffer);
    ~CommandBuffer();

    [[nodiscard]] VkCommandBuffer get() const noexcept
    {
        return _vkCommandBuffer;
    }

    // Begin command buffer to record new commands
    void beginCommandBuffer(uint32_t swapChainIndex, uint32_t frameIndex,
                            uint32_t backBufferIndex,
                            const std::string& debugName);
    
    // End command buffer recording
    void endCommandBuffer();

    // Begin RenderPass scope
    void beginRenderPass(const RenderTargetsInfo& rtInfo);

    // End RenderPass scope
    void endRenderPass();

    // Bind pipeline to the command buffer
    void bindPipeline(const std::shared_ptr<BasePipeline>& pipeline);

    // Set viewport
    void setViewport(const glm::uvec2& viewportSize);

    // Make final preparation to present recorded result to swapChain back buffer.
    void makeSwapChainFinalLayout(const std::shared_ptr<SwapChain>& swapChain);

    inline const FenceObject& getFenceToSignal() const
    {
        return _fenceToSignal;
    }

 private:
    Queue* _commandQueue = nullptr;
    // CommandPool* _ownerCommandPool = nullptr;
    std::shared_ptr<RenderPass> _boundRenderPass = nullptr;
    RenderTargetsInfo _boundRenderTargetsInfo;
    FenceObject _fenceToSignal;
    VkCommandBuffer _vkCommandBuffer = VK_NULL_HANDLE;
    uint32_t _swapChainIndexCached = UINT32_MAX;
    uint32_t _frameIndexCached = UINT32_MAX;
    uint32_t _backBufferIndexCached = UINT32_MAX;
    std::string _debugName;
    bool _hasBegun = false;
};
}  // namespace VoxFlow

#endif