// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_BUFFER_HPP
#define VOXEL_FLOW_COMMAND_BUFFER_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <string>
#include <array>

namespace VoxFlow
{
class Queue;
class CommandPool;
class RenderPass;
class SwapChain;
class BasePipeline;
class BindableResourceView;

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
    void bindPipeline(const std::shared_ptr<BasePipeline>& pipeline);

    // Set viewport
    void setViewport(const glm::uvec2& viewportSize);

    // Make final preparation to present recorded result to swapChain back buffer.
    void makeSwapChainFinalLayout(const std::shared_ptr<SwapChain>& swapChain);

    // Bind given resource group to command buffer according to descriptor set layout informations
    void bindResourceGroup(
        SetSlotCategory setSlotCategory,
        std::vector<std::pair<std::string_view, BindableResourceView*>>&&
            bindGroup);

    // Commit pending resource bindings to command buffer
    void commitPendingResourceBindings();

    const FenceObject& getFenceToSignal() const
    {
        return _fenceToSignal;
    }

    void drawIndexed(uint32_t indexCount, uint32_t instanceCount,
                     uint32_t firstIndex, int32_t vertexOffset,
                     uint32_t firstInstance);

private:
    // Set pipeline barrier or transition for given resource to desired layout
    void makeResourceLayout(
        BindableResourceView* resourceView,
        const DescriptorSetLayoutDesc::DescriptorType& descriptorDesc);

 private:
    LogicalDevice* _logicalDevice = nullptr;
    std::shared_ptr<RenderPass> _boundRenderPass = nullptr;
    std::shared_ptr<BasePipeline> _boundPipeline = nullptr;
    RenderTargetsInfo _boundRenderTargetsInfo;
    FenceObject _fenceToSignal = FenceObject::Default();
    VkCommandBuffer _vkCommandBuffer = VK_NULL_HANDLE;
    std::array<std::vector<std::pair<std::string_view, BindableResourceView*>>,
               MAX_NUM_SET_SLOTS>
        _pendingResourceBindings;
    FrameContext _frameContext;
    std::string _debugName;
    bool _hasBegun = false;
};
}  // namespace VoxFlow

#endif