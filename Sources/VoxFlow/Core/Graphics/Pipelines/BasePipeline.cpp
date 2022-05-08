// Author : snowapril

#include <spdlog/spdlog.h>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>

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

BasePipeline::BasePipeline(const std::shared_ptr<LogicalDevice>& device, VkPipelineLayout layout)
    : _device(device), _layout(layout)
{
    // Do nothing
}

BasePipeline::~BasePipeline()
{
    release();
}

BasePipeline::BasePipeline(BasePipeline&& other) noexcept
    : _device(std::move(other._device)), _pipeline(other._pipeline)
{
    // Do nothing
}

BasePipeline& BasePipeline::operator=(BasePipeline&& other) noexcept
{
    if (&other != this)
    {
        _device = std::move(other._device);
        _pipeline = other._pipeline;
    }
    return *this;
}

void BasePipeline::bindPipeline(const CommandBuffer& cmdBuffer) const noexcept
{
    vkCmdBindPipeline(cmdBuffer.get(), getBindPoint(), _pipeline);
}

void BasePipeline::release()
{
    if (_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(_device->get(), _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
    }
    if (_layout != VK_NULL_HANDLE)
    {
        vkDestroyPipelineLayout(_device->get(), _layout, nullptr);
        _layout = VK_NULL_HANDLE;
    }
}

VkPipelineShaderStageCreateInfo BasePipeline::compileToShaderStage(
    const char* filename)
{
    std::vector<char> shaderSource;
    VK_ASSERT(GlslangUtil::ReadShaderFile(filename, &shaderSource) == true);

    const glslang_stage_t glslangStage =
        GlslangUtil::GlslangStageFromFilename(filename);
    std::vector<unsigned int> spirvBinary;
    VK_ASSERT(GlslangUtil::CompileShader(glslangStage, shaderSource.data(),
                                         &spirvBinary));

    [[maybe_unused]] auto moduleInfo =
        Initializer::MakeInfo<VkShaderModuleCreateInfo>();
    moduleInfo.codeSize = spirvBinary.size() * sizeof(unsigned int);
    moduleInfo.pCode = spirvBinary.data();

    VkShaderModule module = VK_NULL_HANDLE;
    VK_ASSERT(vkCreateShaderModule(_device->get(), &moduleInfo, nullptr,
                                   &module) == VK_SUCCESS);

    auto stageCreateInfo =
        Initializer::MakeInfo<VkPipelineShaderStageCreateInfo>();
    stageCreateInfo.stage =
        GlslangUtil::GlslangStageToVulkanStage(glslangStage);
    stageCreateInfo.module = module;
    return stageCreateInfo;
}
}  // namespace VoxFlow