// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>

namespace VoxFlow
{
GraphicsPipeline::GraphicsPipeline(
    const std::shared_ptr<LogicalDevice>& device,
    const std::vector<const char*>& shaderFilenames,
    const PipelineCreateInfo& createInfo)
    : BasePipeline(device, shaderFilenames, createInfo)
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

}  // namespace VoxFlow