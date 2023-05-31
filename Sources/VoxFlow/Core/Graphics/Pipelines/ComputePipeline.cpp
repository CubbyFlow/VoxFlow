// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
ComputePipeline::ComputePipeline(LogicalDevice* logicalDevice,
                                 std::shared_ptr<ShaderModule> shaderModule,
                                 const std::shared_ptr<PipelineLayout>& layout)
    : BasePipeline(logicalDevice, layout, { shaderModule })
{
    auto stageCreateInfo =
        Initializer::MakeInfo<VkPipelineShaderStageCreateInfo>();
    stageCreateInfo.stage = shaderModule->getStageFlagBits();
    stageCreateInfo.module = shaderModule->get();
    stageCreateInfo.pName = "main"; // TODO(snowapril) : make entrypoint controllable.

    [[maybe_unused]] const VkComputePipelineCreateInfo pipelineInfo = {
        .sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .stage = stageCreateInfo,
        .layout = _layout->get(),
        .basePipelineHandle = VK_NULL_HANDLE,
        .basePipelineIndex = -1
    };

    VK_ASSERT(vkCreateComputePipelines(_logicalDevice->get(), VK_NULL_HANDLE, 1,
                                       &pipelineInfo, nullptr, &_pipeline));
}

ComputePipeline::~ComputePipeline()
{
    // Do nothing
}

ComputePipeline::ComputePipeline(ComputePipeline&& other) noexcept
    : BasePipeline(std::move(other))
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

}  // namespace VoxFlow