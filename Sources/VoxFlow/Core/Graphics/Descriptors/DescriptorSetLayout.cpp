// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetLayout.hpp>
#include <VoxFlow/Core/Utils/Initializer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <algorithm>

namespace VoxFlow
{

DescriptorSetLayout::DescriptorSetLayout(
    LogicalDevice* logicalDevice,
    const std::vector<ShaderLayoutBinding>& shaderLayoutBindings)
    : _logicalDevice(logicalDevice), _shaderLayoutBindings(shaderLayoutBindings)
{
    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings;
    setLayoutBindings.reserve(_shaderLayoutBindings.size());

    uint32_t prevSetNumber = UINT32_MAX;
    std::for_each(
        shaderLayoutBindings.begin(), shaderLayoutBindings.end(),
        [&setLayoutBindings,
         &prevSetNumber](const ShaderLayoutBinding& setLayoutBinding) {
            if (prevSetNumber != UINT32_MAX)
            {
                prevSetNumber = setLayoutBinding._set;
            }
            else
            {
                VOX_ASSERT(prevSetNumber == setLayoutBinding._set,
                           "ShaderLayoutBindings in DescriptorSetLayout must "
                           "have same set number");
            }

            setLayoutBindings.push_back(setLayoutBinding._vkLayoutBindnig);
        });

    VkDescriptorSetLayoutCreateInfo layoutInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(setLayoutBindings.size()),
        .pBindings = setLayoutBindings.data()
    };

    VK_ASSERT(vkCreateDescriptorSetLayout(_logicalDevice->get(), &layoutInfo,
                                          nullptr, &_setLayout));
}

DescriptorSetLayout::~DescriptorSetLayout()
{
    if (_setLayout != VK_NULL_HANDLE)
    {
        vkDestroyDescriptorSetLayout(_logicalDevice->get(), _setLayout,
                                     nullptr);
    }
}

DescriptorSetLayout::DescriptorSetLayout(DescriptorSetLayout&& other) noexcept
{
    operator=(std::move(other));
}
DescriptorSetLayout& DescriptorSetLayout::operator=(
    DescriptorSetLayout&& other) noexcept
{
    if (this != &other)
    {
        _logicalDevice = other._logicalDevice;
        _shaderLayoutBindings.swap(other._shaderLayoutBindings);
        _setLayout = other._setLayout;
        other._setLayout = VK_NULL_HANDLE;
    }
    return *this;
}
}  // namespace VoxFlow