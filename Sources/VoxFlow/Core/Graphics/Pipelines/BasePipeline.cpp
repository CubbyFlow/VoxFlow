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
BasePipeline::BasePipeline(const std::shared_ptr<LogicalDevice>& device,
                           const std::vector<const char*>& shaderFilenames,
                           const PipelineCreateInfo& createInfo)
    : _device(device)
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
    for (const auto& filename : shaderFilenames)
    {
        const glslang_stage_t glslangStage =
            GlslangUtil::GlslangStageFromFilename(filename);
        std::vector<unsigned int> spirvBinary;
        VK_ASSERT(
            GlslangUtil::CompileShader(glslangStage, filename, &spirvBinary));

        auto moduleInfo = Initializer::MakeInfo<VkShaderModuleCreateInfo>();
        moduleInfo.codeSize = spirvBinary.size() * sizeof(unsigned int);
        moduleInfo.pCode = spirvBinary.data();

        VkShaderModule module;
        VK_ASSERT(vkCreateShaderModule(device->get(), &moduleInfo, nullptr,
                                       &module) == VK_SUCCESS);

        auto stageCreateInfo =
            Initializer::MakeInfo<VkPipelineShaderStageCreateInfo>();
        stageCreateInfo.stage =
            GlslangUtil::GlslangStageToVulkanStage(glslangStage);
        stageCreateInfo.module = module;
        shaderStages.emplace_back(stageCreateInfo);
    }

    // TODO(snowapril) : replace layout, renderPass to parameters
    const VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = static_cast<unsigned int>(shaderStages.size()),
        .pStages = shaderStages.data(),
        .pVertexInputState = &createInfo.vertexInputState,
        .pInputAssemblyState = &createInfo.inputAssemblyState,
        .pTessellationState = &createInfo.tessellationState,
        .pViewportState = &createInfo.viewportState,
        .pRasterizationState = &createInfo.rasterizationState,
        .pMultisampleState = &createInfo.multisampleState,
        .pDepthStencilState = &createInfo.depthStencilState,
        .pColorBlendState = &createInfo.colorBlendState,
        .pDynamicState = &createInfo.dynamicState,
        .layout = createInfo.layout,
        .renderPass = createInfo.renderPass,
        .subpass = createInfo.subpass,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    VK_ASSERT(vkCreateGraphicsPipelines(_device->get(), VK_NULL_HANDLE, 1,
                                        &pipelineInfo, nullptr,
                                        &_pipeline) == VK_SUCCESS);
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
}
}  // namespace VoxFlow