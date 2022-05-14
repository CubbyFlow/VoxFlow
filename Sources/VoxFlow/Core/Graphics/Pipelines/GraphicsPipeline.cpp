// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <execution>

namespace VoxFlow
{
GraphicsPipeline::GraphicsPipeline(
    const std::shared_ptr<LogicalDevice>& device,
    const std::vector<const char*>& shaderFilenames,
    const PipelineCreateInfo& createInfo,
    const std::shared_ptr<PipelineLayout>& layout)
    : BasePipeline(device, layout)
{
    std::for_each(
        std::execution::par, shaderFilenames.begin(), shaderFilenames.end(),
        [this](const char* filename) {
            _shaderStageInfos.emplace_back(compileToShaderStage(filename));
        });

    [[maybe_unused]] const VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stageCount = static_cast<unsigned int>(_shaderStageInfos.size()),
        .pStages = _shaderStageInfos.data(),
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

    VK_ASSERT(vkCreateGraphicsPipelines(_device->get(), VK_NULL_HANDLE, 1,
                                        &pipelineInfo, nullptr,
                                        &_pipeline));
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