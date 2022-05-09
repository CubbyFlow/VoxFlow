// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{
PipelineLayout::PipelineLayout(const std::shared_ptr<LogicalDevice>& device)
    : _device(device)
{
    [[maybe_unused]] const auto layoutInfo =
        Initializer::MakeInfo<VkPipelineLayoutCreateInfo>();

    // TODO(snowapril) : add descriptor layout, push constant ranges to layout info

    VK_ASSERT(vkCreatePipelineLayout(_device->get(), &layoutInfo, nullptr,
                                     &_layout) == VK_SUCCESS);
}

PipelineLayout::~PipelineLayout()
{
    release();
}

PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
    : _device(std::move(other._device)), _layout(other._layout)
{
    // Do nothing
}

PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other) noexcept
{
    if (&other != this)
    {
        _device = std::move(other._device);
        _layout = other._layout;
    }
    return *this;
}

void PipelineLayout::release()
{
    if (_layout)
    {
        vkDestroyPipelineLayout(_device->get(), _layout, nullptr);
        _layout = VK_NULL_HANDLE;
    }
    _device.reset();
}
}  // namespace VoxFlow