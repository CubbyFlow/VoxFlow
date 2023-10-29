// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_BUFFER_HPP
#define VOXEL_FLOW_COMMAND_BUFFER_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/ResourceBarrierManager.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ResourceBindingLayout.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassParams.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderTargetGroup.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <array>
#include <string>

namespace VoxFlow
{
class Queue;
class Buffer;
class Texture;
class BufferView;
class TextureView;
class StagingBuffer;
class CommandPool;
class RenderPass;
class SwapChain;
class BasePipeline;
class ResourceView;
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
    void beginCommandBuffer(const FenceObject& fenceToSignal, const std::string& debugName);

    // End command buffer recording
    void endCommandBuffer();

    // Begin RenderPass scope
    void beginRenderPass(const AttachmentGroup& attachmentGroup, const RenderPassParams& passParams);

    // End RenderPass scope
    void endRenderPass();

    /**
     * @param vertexBuffer to bind as vertex buffer usage
     */
    void bindVertexBuffer(Buffer* vertexBuffer);

    /**
     * @param indexBuffer to bind as index buffer usage
     */
    void bindIndexBuffer(Buffer* indexBuffer);

    // Bind pipeline to the command buffer
    void bindPipeline(BasePipeline* pipeline);

    // Unbind current bound pipeline
    void unbindPipeline();

    // Set viewport
    void setViewport(const glm::uvec2& viewportSize);

    // Make final preparation to present recorded result to swapChain back
    // buffer.
    void makeSwapChainFinalLayout(SwapChain* swapChain, const uint32_t backBufferIndex);

    // Bind given resource group to command buffer according to descriptor set
    // layout informations
    void bindResourceGroup(SetSlotCategory setSlotCategory, std::vector<ShaderVariableBinding>&& bindGroup);

    // Commit pending resource bindings to command buffer
    void commitPendingResourceBindings();

    const FenceObject& getFenceToSignal() const
    {
        return _fenceToSignal;
    }

    void uploadBuffer(Buffer* dstBuffer, StagingBuffer* srcBuffer, const uint32_t dstOffset, const uint32_t srcOffset, const uint32_t size);

    void uploadTexture(Texture* dstTexture, StagingBuffer* srcBuffer, const uint32_t dstOffset, const uint32_t srcOffset, const uint32_t size);

    void draw(uint32_t vertexCount, uint32_t instanceCount, uint32_t firstVertex, uint32_t firstInstance);

    void drawIndexed(uint32_t indexCount, uint32_t instanceCount, uint32_t firstIndex, int32_t vertexOffset, uint32_t firstInstance);

    void addGlobalMemoryBarrier(ResourceAccessMask prevAccessMasks, ResourceAccessMask nextAccessMasks);

    void addMemoryBarrier(ResourceView* view, ResourceAccessMask accessMask, VkPipelineStageFlags nextStages = VK_PIPELINE_STAGE_NONE);

    void addExecutionBarrier(VkPipelineStageFlags prevStages, VkPipelineStageFlags nextStages);

 private:
    LogicalDevice* _logicalDevice = nullptr;
    RenderPass* _boundRenderPass = nullptr;
    BasePipeline* _boundPipeline = nullptr;
    RenderTargetsInfo _boundRenderTargetsInfo;
    FenceObject _fenceToSignal = FenceObject::Default();
    VkCommandBuffer _vkCommandBuffer = VK_NULL_HANDLE;
    std::array<std::vector<ShaderVariableBinding>, MAX_NUM_SET_SLOTS> _pendingResourceBindings;
    std::string _debugName;
    bool _hasBegun = false;

    // TODO(snowapril) : temporary member variable
    class Sampler* _sampler = nullptr;

    ResourceBarrierManager _resourceBarrierManager;
    bool _isInRenderPassScope = false;
};
}  // namespace VoxFlow

#endif