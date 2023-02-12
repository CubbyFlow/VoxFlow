// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
GraphicsPipeline::GraphicsPipeline(
    LogicalDevice* logicalDevice,
    std::vector<std::shared_ptr<ShaderModule>>&& shaderModules,
    const PipelineCreateInfo& createInfo,
    const std::shared_ptr<PipelineLayout>& layout)
    : BasePipeline(logicalDevice, layout, std::move(shaderModules))
{
    std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;
    shaderStageInfos.reserve(_shaderModules.size());

    std::for_each(
        _shaderModules.begin(), _shaderModules.end(),
        [&shaderStageInfos](const std::shared_ptr<ShaderModule>& module) {
            auto stageCreateInfo =
                Initializer::MakeInfo<VkPipelineShaderStageCreateInfo>();
            stageCreateInfo.stage = module->getStageFlagBits();
            stageCreateInfo.module = module->get();
            shaderStageInfos.push_back(stageCreateInfo);
        });

    [[maybe_unused]] const VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = static_cast<unsigned int>(shaderStageInfos.size()),
        .pStages = shaderStageInfos.data(),
        .pVertexInputState = &createInfo.vertexInputState,
        .pInputAssemblyState = &createInfo.inputAssemblyState,
        .pTessellationState = &createInfo.tessellationState,
        .pViewportState = &createInfo.viewportState,
        .pRasterizationState = &createInfo.rasterizationState,
        .pMultisampleState = &createInfo.multisampleState,
        .pDepthStencilState = &createInfo.depthStencilState,
        .pColorBlendState = &createInfo.colorBlendState,
        .pDynamicState = &createInfo.dynamicState,
        .layout = _layout->get(),
        .renderPass = createInfo.renderPass,
        .subpass = createInfo.subpass,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    VK_ASSERT(vkCreateGraphicsPipelines(_logicalDevice->get(), VK_NULL_HANDLE, 1,
                                        &pipelineInfo, nullptr, &_pipeline));
}

GraphicsPipeline::~GraphicsPipeline()
{
    // Do nothing
}

GraphicsPipeline::GraphicsPipeline(GraphicsPipeline&& other) noexcept
    : BasePipeline(std::move(other))
{
    // Do nothing
}

GraphicsPipeline& GraphicsPipeline::operator=(GraphicsPipeline&& other) noexcept
{
    if (&other != this)
    {
        BasePipeline::operator=(std::move(other));
    }
    return *this;
}

}  // namespace VoxFlow