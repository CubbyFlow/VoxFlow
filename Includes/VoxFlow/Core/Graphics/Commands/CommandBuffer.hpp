// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_BUFFER_HPP
#define VOXEL_FLOW_COMMAND_BUFFER_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ResourceBindingLayout.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <string>
#include <array>

namespace VoxFlow
{
class Queue;
class Buffer;
class Texture;
class StagingBuffer;
class CommandPool;
class RenderPass;
class SwapChain;
class BasePipeline;
class BindableResourceView;
class LogicalDevice;

class CommandBuffer : private NonCopyable
{
 public:
    explicit CommandBuffer(LogicalDevice* logicalDevice, VkCommandBuffer vkCommandBuffer);
    ~CommandBuffer();

    [[nodiscard]] VkCommandBuffer get() const noexcept
    {
        return _vkCommandBuffer;
    }

    // Begin command buffer to record new commands
    void beginCommandBuffer(const FrameContext& frameContext,
                            const FenceObject& fenceToSignal,
                            const std::string& debugName);
    
    // End command buffer recording
    void endCommandBuffer();

    // Begin RenderPass scope
    void beginRenderPass(const RenderTargetsInfo& rtInfo);

    // End RenderPass scope
    void endRenderPass();

    // Bind pipeline to the command buffer
    void bindPipeline(BasePipeline* pipeline);

    // Unbind current bound pipeline
    void unbindPipeline();

    // Set viewport
    void setViewport(const glm::uvec2& viewportSize);

    // Make final preparation to present recorded result to swapChain back buffer.
    void makeSwapChainFinalLayout(SwapChain* swapChain);

    // Bind given resource group to command buffer according to descriptor set layout informations
    void bindResourceGroup(
        SetSlotCategory setSlotCategory,
        std::vector<ShaderVariable>&&
            bindGroup);

    // Commit pending resource bindings to command buffer
    void commitPendingResourceBindings();

    const FenceObject& getFenceToSignal() const
    {
        return _fenceToSignal;
    }

    void uploadBuffer(Buffer* dstBuffer, StagingBuffer* srcBuffer,
                      const uint32_t dstOffset, const uint32_t srcOffset,
                      const uint32_t size);

    void uploadTexture(Texture* dstTexture, StagingBuffer* srcBuffer,
                       const uint32_t dstOffset, const uint32_t srcOffset,
                       const uint32_t size);

    void drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, int32_t vertexOffset,
                     uint32_t firstInstance);

 private:
    // Set pipeline barrier or transition for given resource to desired layout
    void makeResourceLayout(BindableResourceView* resourceView,
                            const DescriptorInfo& descInfo);

 private:
    LogicalDevice* _logicalDevice = nullptr;
    RenderPass* _boundRenderPass = nullptr;
    BasePipeline* _boundPipeline = nullptr;
    RenderTargetsInfo _boundRenderTargetsInfo;
    FenceObject _fenceToSignal = FenceObject::Default();
    VkCommandBuffer _vkCommandBuffer = VK_NULL_HANDLE;
    std::array<std::vector<ShaderVariable>, MAX_NUM_SET_SLOTS>
        _pendingResourceBindings;
    FrameContext _frameContext;
    std::string _debugName;
    bool _hasBegun = false;
};
}  // namespace VoxFlow

#endif