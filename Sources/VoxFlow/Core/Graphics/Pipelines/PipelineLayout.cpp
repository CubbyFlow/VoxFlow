// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <algorithm>

namespace VoxFlow
{
PipelineLayout::PipelineLayout(
    LogicalDevice* logicalDevice,
    const std::vector<std::shared_ptr<DescriptorSetLayout>>& setLayouts)
    : _logicalDevice(logicalDevice), _setLayouts(setLayouts)
{
    auto layoutInfo = Initializer::MakeInfo<VkPipelineLayoutCreateInfo>();

    std::vector<VkDescriptorSetLayout> vkSetLayouts;
    vkSetLayouts.reserve(_setLayouts.size());

    std::for_each(
        _setLayouts.begin(), _setLayouts.end(),
        [&vkSetLayouts](const std::shared_ptr<DescriptorSetLayout>& setLayout) {
            vkSetLayouts.push_back(setLayout->get());
        });

    layoutInfo.setLayoutCount = static_cast<uint32_t>(vkSetLayouts.size());
    layoutInfo.pSetLayouts = vkSetLayouts.data();

    VK_ASSERT(vkCreatePipelineLayout(_logicalDevice->get(), &layoutInfo,
                                     nullptr, &_layout));
}

PipelineLayout::~PipelineLayout()
{
    release();
}

PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
    : _logicalDevice(std::move(other._logicalDevice)), _layout(other._layout)
{
    // Do nothing
}

PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = std::move(other._logicalDevice);
        _layout = other._layout;
    }
    return *this;
}

void PipelineLayout::release()
{
    if (_layout)
    {
        vkDestroyPipelineLayout(_logicalDevice->get(), _layout, nullptr);
        _layout = VK_NULL_HANDLE;
    }
}
}  // namespace VoxFlow