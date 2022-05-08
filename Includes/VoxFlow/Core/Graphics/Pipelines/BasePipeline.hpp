// Author : snowapril

#ifndef VOXEL_FLOW_BASE_PIPELINE_HPP
#define VOXEL_FLOW_BASE_PIPELINE_HPP

#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;

struct PipelineCreateInfo
{
    VkPipelineVertexInputStateCreateInfo vertexInputState;
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState;
    VkPipelineTessellationStateCreateInfo tessellationState;
    VkPipelineViewportStateCreateInfo viewportState;
    VkPipelineRasterizationStateCreateInfo rasterizationState;
    VkPipelineMultisampleStateCreateInfo multisampleState;
    VkPipelineDepthStencilStateCreateInfo depthStencilState;
    VkPipelineColorBlendStateCreateInfo colorBlendState;
    VkPipelineDynamicStateCreateInfo dynamicState;
    VkRenderPass renderPass;
    unsigned int subpass;

    static PipelineCreateInfo CreateDefault() noexcept;
};

class BasePipeline : NonCopyable
{
 public:
    explicit BasePipeline(const std::shared_ptr<LogicalDevice>& device,
                          VkPipelineLayout layout);
    ~BasePipeline() override;
    BasePipeline(BasePipeline&& other) noexcept;
    BasePipeline& operator=(BasePipeline&& other) noexcept;

    void bindPipeline(const CommandBuffer& cmdBuffer) const noexcept;

    [[nodiscard]] VkPipeline get() const noexcept
    {
        return _pipeline;
    }

    [[nodiscard]] VkPipelineLayout getLayout() const noexcept
    {
        return _layout;
    }

    [[nodiscard]] virtual VkPipelineBindPoint getBindPoint() const noexcept = 0;

 protected:
    [[nodiscard]] VkPipelineShaderStageCreateInfo compileToShaderStage(
        const char* filename);
    void release();

 protected:
    std::shared_ptr<LogicalDevice> _device;
    VkPipeline _pipeline{ VK_NULL_HANDLE };
    VkPipelineLayout _layout{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif