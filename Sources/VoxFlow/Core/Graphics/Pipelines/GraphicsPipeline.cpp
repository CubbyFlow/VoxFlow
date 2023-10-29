// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineCache.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
GraphicsPipeline::GraphicsPipeline(
    PipelineStreamingContext* pipelineStreamingContext,
    std::vector<ShaderPathInfo>&& shaderPaths)
    : BasePipeline(pipelineStreamingContext, std::move(shaderPaths))
{
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

bool GraphicsPipeline::initialize(RenderPass* renderPass)
{
    _pipelineLayout = std::make_unique<PipelineLayout>(_logicalDevice);

    std::vector<const ShaderReflectionDataGroup*> combinedReflectionDataGroups;

    const size_t numShaderModules = _shaderModules.size();
    combinedReflectionDataGroups.reserve(numShaderModules);

    for (size_t i = 0; i < numShaderModules; ++i)
    {
        combinedReflectionDataGroups.push_back(
            _shaderModules[i]->getShaderReflectionDataGroup());
    }

    if (_pipelineLayout->initialize(combinedReflectionDataGroups) == false)
    {
        VOX_ASSERT(false, "Failed to create pipeline layout");
        return false;
    }

    // TODO(snowapril) : need comparison between input layout and reflected one
    //std::vector<VertexInputLayout> vertexInputLayouts;
    //std::vector<FragmentOutputLayout> fragmentOutputLayouts;
    //
    //for (const ShaderReflectionDataGroup* reflectionDataGroup :
    //     combinedReflectionDataGroups)
    //{
    //    if (vertexInputLayouts.empty() &&
    //        (reflectionDataGroup->_vertexInputLayouts.empty() == false))
    //    {
    //        std::move(reflectionDataGroup->_vertexInputLayouts.begin(),
    //                  reflectionDataGroup->_vertexInputLayouts.end(),
    //                  std::back_inserter(vertexInputLayouts));
    //    }
    //
    //    if (fragmentOutputLayouts.empty() &&
    //        (reflectionDataGroup->_fragmentOutputLayouts.empty() == false))
    //    {
    //        std::move(reflectionDataGroup->_fragmentOutputLayouts.begin(),
    //                  reflectionDataGroup->_fragmentOutputLayouts.end(),
    //                  std::back_inserter(fragmentOutputLayouts));
    //    }
    //}

    std::vector<VkPipelineShaderStageCreateInfo> shaderStageInfos;
    shaderStageInfos.reserve(_shaderModules.size());

    std::for_each(
        _shaderModules.begin(), _shaderModules.end(),
        [&shaderStageInfos](const std::unique_ptr<ShaderModule>& module) {
            const VkPipelineShaderStageCreateInfo stageCreateInfo = {
                .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
                .pNext = nullptr,
                .flags = 0,
                .stage = module->getStageFlagBits(),
                .module = module->get(),
                .pName = "main",
                .pSpecializationInfo = nullptr
            };
            shaderStageInfos.push_back(stageCreateInfo);
        });

    std::vector<VkVertexInputBindingDescription> bindingDescriptions;
    std::vector<VkVertexInputAttributeDescription> attributeDescriptions;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.pNext = nullptr;

    // TODO(snowapril) : support instancing with given input layout
    const std::vector<VertexInputLayout>& inputLayouts =
        _pipelineState.inputLayout.inputLayouts;
    const bool hasStageInputs = inputLayouts.size() > 0;
    if (hasStageInputs)
    {
        uint32_t offset = 0;
        for (const VertexInputLayout& inputLayout : inputLayouts)
        {
            bindingDescriptions.push_back(
                { .binding = 0,
                  .stride = inputLayout._stride,
                  .inputRate = VK_VERTEX_INPUT_RATE_VERTEX });

            attributeDescriptions.push_back(
                { .location = inputLayout._location,
                  .binding = 0,
                  .format = inputLayout.getVkFormat(),
                  .offset = offset });
            
            offset += inputLayout._stride;
        }

        vertexInputInfo.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(attributeDescriptions.size());
        vertexInputInfo.pVertexAttributeDescriptions =
            attributeDescriptions.data();
        vertexInputInfo.vertexBindingDescriptionCount =
            static_cast<uint32_t>(bindingDescriptions.size());
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data();
    }
    else
    {
        vertexInputInfo.vertexAttributeDescriptionCount = 0;
        vertexInputInfo.pVertexAttributeDescriptions = nullptr;
        vertexInputInfo.vertexBindingDescriptionCount = 0;
        vertexInputInfo.pVertexBindingDescriptions = nullptr;
    }

    VkPipelineInputAssemblyStateCreateInfo
        inputAssemblyInfo = {};
    inputAssemblyInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.pNext = nullptr;
    inputAssemblyInfo.topology = _pipelineState.topology;
    inputAssemblyInfo.flags = 0;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    VkPipelineTessellationStateCreateInfo tessellationInfo = {};
    tessellationInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
    tessellationInfo.pNext = nullptr;
    tessellationInfo.flags = 0;
    tessellationInfo.patchControlPoints = 0;

    VkPipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.pNext = nullptr;
    viewportInfo.flags = 0;
    viewportInfo.viewportCount = 1;
    viewportInfo.pViewports = nullptr;
    viewportInfo.scissorCount = 1;
    viewportInfo.pScissors = nullptr;

    VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
    rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.depthClampEnable = VK_FALSE;
    rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;
    rasterizationInfo.polygonMode = _pipelineState.rasterization.polygonMode;
    rasterizationInfo.lineWidth = 1.0f;
    rasterizationInfo.cullMode = _pipelineState.rasterization.cullMode;
    rasterizationInfo.frontFace = _pipelineState.rasterization.front;
    rasterizationInfo.depthBiasEnable = VK_FALSE;
    rasterizationInfo.depthBiasConstantFactor = 0.0f;  // Optional
    rasterizationInfo.depthBiasClamp = 0.0f;           // Optional
    rasterizationInfo.depthBiasSlopeFactor = 0.0f;     // Optional

    VkPipelineMultisampleStateCreateInfo multiSampleInfo = {};
    multiSampleInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multiSampleInfo.sampleShadingEnable = VK_FALSE;
    multiSampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multiSampleInfo.minSampleShading = 1.0f;           // Optional
    multiSampleInfo.pSampleMask = nullptr;             // Optional
    multiSampleInfo.alphaToCoverageEnable = VK_FALSE;  // Optional
    multiSampleInfo.alphaToOneEnable = VK_FALSE;       // Optional

    std::vector<VkPipelineColorBlendAttachmentState> colorBlendAttachments;
    colorBlendAttachments.resize(_pipelineState.blendState.activeStates);
    for (uint32_t i = 0; i < _pipelineState.blendState.activeStates; ++i)
    {
        const BlendState& blendState =
            _pipelineState.blendState.blendStates
                [i];

        VkPipelineColorBlendAttachmentState& colorBlendAttachmentState =
            colorBlendAttachments[i];
        colorBlendAttachmentState.colorWriteMask = blendState.colorMasks;
        colorBlendAttachmentState.blendEnable = blendState.blendEnabled ? VK_TRUE : VK_FALSE;
        colorBlendAttachmentState.srcColorBlendFactor = blendState.sourceColor;  // Optional
        colorBlendAttachmentState.dstColorBlendFactor = blendState.destinationColor;  // Optional
        colorBlendAttachmentState.colorBlendOp = blendState.colorOperation;  // Optional
        colorBlendAttachmentState.srcAlphaBlendFactor = blendState.sourceAlpha;  // Optional
        colorBlendAttachmentState.dstAlphaBlendFactor = blendState.destinationAlpha;  // Optional
        colorBlendAttachmentState.alphaBlendOp = blendState.alphaOperation;  // Optional
    }

    VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
    colorBlendInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOpEnable = VK_FALSE;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    colorBlendInfo.attachmentCount =
        static_cast<uint32_t>(colorBlendAttachments.size());
    colorBlendInfo.pAttachments = colorBlendAttachments.data();
    colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
    colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
    colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
    colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
    depthStencilInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = _pipelineState.depthStencil.depthEnable ? VK_TRUE : VK_FALSE;
    depthStencilInfo.depthWriteEnable = _pipelineState.depthStencil.depthWriteEnable ? VK_TRUE : VK_FALSE;
    depthStencilInfo.depthCompareOp = _pipelineState.depthStencil.depthComparison;
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    depthStencilInfo.stencilTestEnable = _pipelineState.depthStencil.stencilEnable;
    depthStencilInfo.front = {
        .failOp = _pipelineState.depthStencil.front.fail,
        .passOp = _pipelineState.depthStencil.front.pass,
        .depthFailOp = _pipelineState.depthStencil.front.depthFail,
        .compareOp = _pipelineState.depthStencil.front.compare,
        .compareMask = _pipelineState.depthStencil.front.compareMask,
        .writeMask = _pipelineState.depthStencil.front.writeMask,
        .reference = _pipelineState.depthStencil.front.reference,
    };
    depthStencilInfo.back = {
        .failOp = _pipelineState.depthStencil.back.fail,
        .passOp = _pipelineState.depthStencil.back.pass,
        .depthFailOp = _pipelineState.depthStencil.back.depthFail,
        .compareOp = _pipelineState.depthStencil.back.compare,
        .compareMask = _pipelineState.depthStencil.back.compareMask,
        .writeMask = _pipelineState.depthStencil.back.writeMask,
        .reference = _pipelineState.depthStencil.back.reference,
    };

    VkDynamicState dynamicStates[] = { VK_DYNAMIC_STATE_VIEWPORT,
                                      VK_DYNAMIC_STATE_SCISSOR };

    VkPipelineDynamicStateCreateInfo dynamicInfo = {};
    dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicInfo.pDynamicStates = dynamicStates;
    dynamicInfo.dynamicStateCount =
        sizeof(dynamicStates) / sizeof(VkDynamicState);
    dynamicInfo.flags = 0;

    const VkGraphicsPipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = _pipelineState.flags,
        .stageCount = static_cast<unsigned int>(shaderStageInfos.size()),
        .pStages = shaderStageInfos.data(),
        .pVertexInputState = &vertexInputInfo,
        .pInputAssemblyState = &inputAssemblyInfo,
        .pTessellationState = &tessellationInfo,
        .pViewportState = &viewportInfo,
        .pRasterizationState = &rasterizationInfo,
        .pMultisampleState = &multiSampleInfo,
        .pDepthStencilState = &depthStencilInfo,
        .pColorBlendState = &colorBlendInfo,
        .pDynamicState = &dynamicInfo,
        .layout = _pipelineLayout->get(),
        .renderPass = renderPass->get(),
        .subpass = 0,
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    VkPipelineCache pipelineCache =
        _pipelineCache != nullptr ? _pipelineCache->get() : VK_NULL_HANDLE;
    VK_ASSERT(vkCreateGraphicsPipelines(_logicalDevice->get(), pipelineCache, 1,
                                        &pipelineInfo, nullptr, &_pipeline));

    if (_pipeline == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to create graphics pipeline");
        return false;
    }

    _pipelineCache.reset();
    
    return true;
}

}  // namespace VoxFlow