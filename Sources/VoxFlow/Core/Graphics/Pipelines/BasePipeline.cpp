// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp>
#include <VoxFlow/Core/Utils/pch.hpp>

namespace VoxFlow
{
BasePipeline::BasePipeline(const std::shared_ptr<LogicalDevice>& device)
    : _device(device)
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

void BasePipeline::release()
{
    if (_pipeline != VK_NULL_HANDLE)
    {
        vkDestroyPipeline(_device->get(), _pipeline, nullptr);
        _pipeline = VK_NULL_HANDLE;
    }
}
}  // namespace VoxFlow