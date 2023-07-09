// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <algorithm>

namespace VoxFlow
{
PipelineLayout::PipelineLayout(
    LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
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
        _combinedSetLayouts.swap(other._combinedSetLayouts);
    }
    return *this;
}

static void organizeCombinedDescSetLayouts(
    std::vector<ShaderLayoutBinding>&& setLayoutBindings,
    DescriptorSetLayoutDesc* pSetLayouts)
{
    for (const ShaderLayoutBinding& shaderBinding : setLayoutBindings)
    {
        for (uint32_t set = 0; set < MAX_NUM_SET_SLOTS; ++set)
        {
            std::for_each(
                shaderBinding._sets[set]._bindingMap.begin(),
                shaderBinding._sets[set]._bindingMap.end(),
                [&pSetLayouts,
                 set](const DescriptorSetLayoutDesc::ContainerType::value_type&
                          bindingPair) {
                    DescriptorSetLayoutDesc::ContainerType::const_iterator it =
                        pSetLayouts[set]._bindingMap.find(bindingPair.first);
                    if (it != pSetLayouts[set]._bindingMap.end())
                    {
                        // TODO(snowapril) : Check given resource is collided
                        // with already collected one.
                    }
                    else
                    {
                        pSetLayouts[set]._bindingMap.emplace(bindingPair);
                    }
                });

            pSetLayouts[set]._stageFlags |=
                shaderBinding._sets[set]._stageFlags;
        }
    }
}

bool PipelineLayout::initialize(std::vector<ShaderLayoutBinding>&& setLayoutBindings)
{
    organizeCombinedDescSetLayouts(std::move(setLayoutBindings),
                                   _combinedSetLayouts.data());

    DescriptorSetAllocatorPool* descriptorSetAllocatorPool =
        _logicalDevice->getDescriptorSetAllocatorPool();

    std::vector<VkDescriptorSetLayout> vkSetLayouts;

    // Set index 0 is always bindless descriptor set.
    _setAllocators[0] =
        descriptorSetAllocatorPool->getBindlessDescriptorSetAllocator();
    for (uint32_t set = 1; set < MAX_NUM_SET_SLOTS; ++set)
    {
        if (_combinedSetLayouts[set]._stageFlags != 0)
        {
            _setAllocators[set] =
                descriptorSetAllocatorPool->getOrCreateDescriptorSetAllocator(
                    _combinedSetLayouts[set]);
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

    return (_vkPipelineLayout != VK_NULL_HANDLE);
}

void PipelineLayout::release()
{
    for (std::shared_ptr<DescriptorSetAllocator>& setAllocator : _setAllocators)
    {
        setAllocator.reset();
    }

    if (_vkPipelineLayout)
    {
        vkDestroyPipelineLayout(_logicalDevice->get(), _vkPipelineLayout, nullptr);
        _vkPipelineLayout = VK_NULL_HANDLE;
    }
}

}  // namespace VoxFlow