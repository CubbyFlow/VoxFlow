// Author : snowapril

#include <spdlog/spdlog.h>

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
PipelineCreateInfo PipelineCreateInfo::CreateDefault() noexcept
{
    return {
        .vertexInputState =
            Initializer::MakeInfo<VkPipelineVertexInputStateCreateInfo>(),
        .inputAssemblyState =
            Initializer::MakeInfo<VkPipelineInputAssemblyStateCreateInfo>(),
        .tessellationState =
            Initializer::MakeInfo<VkPipelineTessellationStateCreateInfo>(),
        .viewportState =
            Initializer::MakeInfo<VkPipelineViewportStateCreateInfo>(),
        .rasterizationState =
            Initializer::MakeInfo<VkPipelineRasterizationStateCreateInfo>(),
        .multisampleState =
            Initializer::MakeInfo<VkPipelineMultisampleStateCreateInfo>(),
        .depthStencilState =
            Initializer::MakeInfo<VkPipelineDepthStencilStateCreateInfo>(),
        .colorBlendState =
            Initializer::MakeInfo<VkPipelineColorBlendStateCreateInfo>(),
        .dynamicState =
            Initializer::MakeInfo<VkPipelineDynamicStateCreateInfo>(),
        .renderPass = VK_NULL_HANDLE,
        .subpass = 0
    };
}

BasePipeline::BasePipeline(
    LogicalDevice* logicalDevice, const std::shared_ptr<PipelineLayout>& layout,
    std::vector<std::shared_ptr<ShaderModule>>&& shaderModules)
    : _logicalDevice(logicalDevice),
      _layout(layout),
      _shaderModules(std::move(shaderModules))
{
    // Do nothing
}

BasePipeline::~BasePipeline()
{
    release();
}

BasePipeline::BasePipeline(BasePipeline&& other) noexcept
{
    operator=(std::move(other));
}

BasePipeline& BasePipeline::operator=(BasePipeline&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = other._logicalDevice;
        _pipeline = other._pipeline;
        other._pipeline = VK_NULL_HANDLE;
    }
    return *this;
}

void BasePipeline::release()
{
    _shaderModules.clear();
    _layout.reset();
    if (_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(_logicalDevice->get(), _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
    }
}

}  // namespace VoxFlow