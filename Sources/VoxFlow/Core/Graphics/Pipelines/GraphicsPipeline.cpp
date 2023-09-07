// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
GraphicsPipeline::GraphicsPipeline(
    LogicalDevice* logicalDevice,
    std::initializer_list<const char*>&& shaderPaths)
    : BasePipeline(logicalDevice, std::move(shaderPaths))
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

    std::vector<VertexInputLayout> vertexInputLayouts;
    std::vector<FragmentOutputLayout> fragmentOutputLayouts;

    for (const ShaderReflectionDataGroup* reflectionDataGroup :
         combinedReflectionDataGroups)
    {
        if (vertexInputLayouts.empty() &&
            (reflectionDataGroup->_vertexInputLayouts.empty() == false))
        {
            std::move(reflectionDataGroup->_vertexInputLayouts.begin(),
                      reflectionDataGroup->_vertexInputLayouts.end(),
                      std::back_inserter(vertexInputLayouts));
        }

        if (fragmentOutputLayouts.empty() &&
            (reflectionDataGroup->_fragmentOutputLayouts.empty() == false))
        {
            std::move(reflectionDataGroup->_fragmentOutputLayouts.begin(),
                      reflectionDataGroup->_fragmentOutputLayouts.end(),
                      std::back_inserter(fragmentOutputLayouts));
        }
    }

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
    const PipelineLayoutDescriptor& pipelineLayoutDesc =
        _pipelineLayout->getPipelineLayoutDescriptor();
    const bool hasStageInputs = vertexInputLayouts.size() > 0;
    if (hasStageInputs)
    {
        uint32_t offset = 0;
        for (const auto& inputLayout : vertexInputLayouts)
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
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
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
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationInfo.lineWidth = 1.0f;
    rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
    rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
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

    VkPipelineColorBlendAttachmentState colorBlendAttachmentState = {};
    colorBlendAttachmentState.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachmentState.blendEnable = VK_FALSE;
    colorBlendAttachmentState.srcColorBlendFactor =
        VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachmentState.dstColorBlendFactor =
        VK_BLEND_FACTOR_ZERO;                                  // Optional
    colorBlendAttachmentState.colorBlendOp = VK_BLEND_OP_ADD;  // Optional
    colorBlendAttachmentState.srcAlphaBlendFactor =
        VK_BLEND_FACTOR_ONE;  // Optional
    colorBlendAttachmentState.dstAlphaBlendFactor =
        VK_BLEND_FACTOR_ZERO;                                  // Optional
    colorBlendAttachmentState.alphaBlendOp = VK_BLEND_OP_ADD;  // Optional

    VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
    colorBlendInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOpEnable = VK_FALSE;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;  // Optional
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendAttachmentState;
    colorBlendInfo.blendConstants[0] = 0.0f;  // Optional
    colorBlendInfo.blendConstants[1] = 0.0f;  // Optional
    colorBlendInfo.blendConstants[2] = 0.0f;  // Optional
    colorBlendInfo.blendConstants[3] = 0.0f;  // Optional

    VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {};
    depthStencilInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = VK_TRUE;
    depthStencilInfo.depthWriteEnable = VK_TRUE;
    depthStencilInfo.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencilInfo.depthBoundsTestEnable = VK_FALSE;
    depthStencilInfo.minDepthBounds = 0.0f;  // Optional
    depthStencilInfo.maxDepthBounds = 1.0f;  // Optional
    depthStencilInfo.stencilTestEnable = VK_FALSE;
    depthStencilInfo.front = {};  // Optional
    depthStencilInfo.back = {};   // Optional

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
        .flags = 0,
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

    VK_ASSERT(vkCreateGraphicsPipelines(_logicalDevice->get(), VK_NULL_HANDLE,
                                        1, &pipelineInfo, nullptr, &_pipeline));

    if (_pipeline == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to create graphics pipeline");
        return false;
    }
    
    return true;
}

}  // namespace VoxFlow