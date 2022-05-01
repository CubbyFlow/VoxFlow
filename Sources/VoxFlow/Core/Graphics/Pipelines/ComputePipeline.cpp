// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp>

namespace VoxFlow
{
ComputePipeline::ComputePipeline(const std::shared_ptr<LogicalDevice>& device)
    : BasePipeline(device)
{
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