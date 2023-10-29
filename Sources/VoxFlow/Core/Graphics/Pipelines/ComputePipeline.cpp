// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineCache.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
ComputePipeline::ComputePipeline(PipelineStreamingContext* pipelineStreamingContext, const ShaderPathInfo& shaderPath)
    : BasePipeline(pipelineStreamingContext, { shaderPath })
{
}

ComputePipeline::~ComputePipeline()
{
    // Do nothing
}

ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept : BasePipeline(std::move(other))
{
    // Do nothing
}

ComputePipeline& ComputePipeline::operator=(ComputePipeline&& other) noexcept
{
    if (&other != this)
    {
        BasePipeline::operator=(std::move(other));
    }
    return *this;
}

bool ComputePipeline::initialize()
{
    _pipelineLayout = std::make_unique<PipelineLayout>(_logicalDevice);

    std::vector<const ShaderReflectionDataGroup*> combinedReflectionDataGroups;

    const size_t numShaderModules = _shaderModules.size();
    combinedReflectionDataGroups.reserve(numShaderModules);

    for (size_t i = 0; i < numShaderModules; ++i)
    {
        combinedReflectionDataGroups.push_back(_shaderModules[i]->getShaderReflectionDataGroup());
    }

    if (_pipelineLayout->initialize(combinedReflectionDataGroups) == false)
    {
        VOX_ASSERT(false, "Failed to create pipeline layout");
        return false;
    }

    ShaderModule* computeShaderModule = _shaderModules.front().get();
    const VkPipelineShaderStageCreateInfo stageCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = VK_SHADER_STAGE_COMPUTE_BIT,
        .module = computeShaderModule->get(),
        .pName = "main",
        .pSpecializationInfo = 0,
    };

    [[maybe_unused]] const VkComputePipelineCreateInfo pipelineInfo = { .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
                                                                        .pNext = nullptr,
                                                                        .flags = 0,
                                                                        .stage = stageCreateInfo,
                                                                        .layout = _pipelineLayout->get(),
                                                                        .basePipelineHandle = VK_NULL_HANDLE,
                                                                        .basePipelineIndex = -1 };

    VkPipelineCache pipelineCache = _pipelineCache != nullptr ? _pipelineCache->get() : VK_NULL_HANDLE;
    VK_ASSERT(vkCreateComputePipelines(_logicalDevice->get(), pipelineCache, 1, &pipelineInfo, nullptr, &_pipeline));

    if (_pipeline == VK_NULL_HANDLE)
    {
        VOX_ASSERT(false, "Failed to create graphics pipeline");
        return false;
    }

    _pipelineCache.reset();

    return true;
}

}  // namespace VoxFlow