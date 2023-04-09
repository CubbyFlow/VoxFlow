// Author : snowapril

#ifndef VOXEL_FLOW_BASE_PIPELINE_HPP
#define VOXEL_FLOW_BASE_PIPELINE_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>

namespace VoxFlow
{
class ShaderModule;
class LogicalDevice;
class PipelineLayout;

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
    explicit BasePipeline(
        LogicalDevice* logicalDevice,
        const std::shared_ptr<PipelineLayout>& layout,
        std::vector<std::shared_ptr<ShaderModule>>&& shaderModules);
    ~BasePipeline() override;
    BasePipeline(BasePipeline&& other) noexcept;
    BasePipeline& operator=(BasePipeline&& other) noexcept;

    [[nodiscard]] VkPipeline get() const noexcept
    {
        return _pipeline;
    }

    [[nodiscard]] std::shared_ptr<PipelineLayout> getLayoutPtr() const noexcept
    {
        return _layout;
    }

    [[nodiscard]] virtual VkPipelineBindPoint getBindPoint() const noexcept = 0;

 protected:
    void release();

 protected:
    LogicalDevice* _logicalDevice = nullptr;
    std::shared_ptr<PipelineLayout> _layout;
    std::vector<std::shared_ptr<ShaderModule>> _shaderModules;
    VkPipeline _pipeline{ VK_NULL_HANDLE };
};
}  // namespace VoxFlow

#endif