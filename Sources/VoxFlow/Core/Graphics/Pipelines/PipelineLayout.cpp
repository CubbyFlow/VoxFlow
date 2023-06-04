// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <algorithm>

namespace VoxFlow
{
PipelineLayout::PipelineLayout(
    LogicalDevice* logicalDevice,
    std::vector<ShaderLayoutBinding>&& setLayoutBindings)
    : _logicalDevice(logicalDevice), _setLayoutBindings(setLayoutBindings)
{
    std::array<DescriptorSetLayoutDesc, MAX_NUM_SET_SLOTS> combinedSetLayouts;
    for (const ShaderLayoutBinding& shaderBinding : setLayoutBindings)
    {
        for (uint32_t set = 0; set < MAX_NUM_SET_SLOTS; ++set)
        {
            std::for_each(
                shaderBinding._sets[set]._bindingMap.begin(),
                shaderBinding._sets[set]._bindingMap.end(),
                [&combinedSetLayouts,
                 set](const DescriptorSetLayoutDesc::ContainerType::value_type&
                          bindingPair) {
                    DescriptorSetLayoutDesc::ContainerType::const_iterator it =
                        combinedSetLayouts[set]._bindingMap.find(
                            bindingPair.first);
                    if (it != combinedSetLayouts[set]._bindingMap.end())
                    {
                        // TODO(snowapril) : Check given resource is collided
                        // with already collected one.
                    }
                    else
                    {
                        combinedSetLayouts[set]._bindingMap.emplace(
                            bindingPair);
                    }
                });
                
            combinedSetLayouts[set]._stageFlags |=
                shaderBinding._sets[set]._stageFlags;
        }
    }

    DescriptorSetAllocatorPool* descriptorSetAllocatorPool =
        _logicalDevice->getDescriptorSetAllocatorPool();

    std::vector<VkDescriptorSetLayout> vkSetLayouts;
    for (uint32_t set = 0; set < MAX_NUM_SET_SLOTS; ++set)
    {
        if (combinedSetLayouts[set]._stageFlags != 0)
        {
            _setAllocators[set] =
                descriptorSetAllocatorPool->getOrCreateDescriptorSetAllocator(
                    combinedSetLayouts[set]);
            vkSetLayouts.push_back(
                _setAllocators[set]->getVkDescriptorSetLayout());
        }
    }

    // TODO(snowapril) : implement push constants
    VkPipelineLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .setLayoutCount = static_cast<uint32_t>(vkSetLayouts.size()),
        .pSetLayouts = vkSetLayouts.data(),
        .pushConstantRangeCount = 0,
        .pPushConstantRanges = nullptr,
    };

    VK_ASSERT(vkCreatePipelineLayout(_logicalDevice->get(), &createInfo,
                                     nullptr, &_vkPipelineLayout));
}

PipelineLayout::~PipelineLayout()
{
    release();
}

PipelineLayout::PipelineLayout(PipelineLayout&& other) noexcept
{
    operator=(std::move(other));
}

PipelineLayout& PipelineLayout::operator=(PipelineLayout&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = std::move(other._logicalDevice);
        _vkPipelineLayout = other._vkPipelineLayout;
        _setAllocators.swap(other._setAllocators);
        _setLayoutBindings.swap(other._setLayoutBindings);
    }
    return *this;
}

void PipelineLayout::release()
{
    if (_vkPipelineLayout)
    {
        vkDestroyPipelineLayout(_logicalDevice->get(), _vkPipelineLayout, nullptr);
        _vkPipelineLayout = VK_NULL_HANDLE;
    }
}
}  // namespace VoxFlow