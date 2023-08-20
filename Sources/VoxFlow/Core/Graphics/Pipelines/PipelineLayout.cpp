// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineLayoutDescriptor.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <algorithm>
#include <unordered_set>

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
        _combinedPipelineLayoutDesc =
            std::move(other._combinedPipelineLayoutDesc);
    }
    return *this;
}

static void organizeCombinedDescSetLayouts(
    std::vector<const ShaderReflectionDataGroup*>&& combinedReflectionGroups,
    PipelineLayoutDescriptor* combinedPipelineLayoutDesc)
{
    // TODO(snowapril) : As each descriptor set layout desc might have same
    // bindings, collision handling must be needed.

    std::unordered_set<uint32_t> collisionCheckSet;
    for (const ShaderReflectionDataGroup* reflectionDataGroup :
         combinedReflectionGroups)
    {
        for (uint32_t set = 0; set < MAX_NUM_SET_SLOTS; ++set)
        {
            for (const auto& [name, shaderVariable] :
                 reflectionDataGroup->_descriptors)
            {
                if (set ==
                    static_cast<uint32_t>(shaderVariable._info._setCategory))
                {
                    const uint32_t key =
                        (static_cast<uint32_t>(
                             shaderVariable._info._descriptorCategory)
                         << 24) |
                        shaderVariable._info._binding;
                    if (collisionCheckSet.find(key) == collisionCheckSet.end())
                    {
                        collisionCheckSet.emplace(key);

                        combinedPipelineLayoutDesc->_sets[set]
                            ._descriptorInfos.emplace_back(
                                shaderVariable._info);

                        combinedPipelineLayoutDesc->_shaderVariablesMap.emplace(
                            name, shaderVariable);
                    }
                    else
                    {
                        // TODO(snowapril) : handling collision. It may not exist at now.
                    }
                }
            }

            combinedPipelineLayoutDesc->_sets[set]._stageFlags |=
                reflectionDataGroup->_stageFlagBit;

            if (combinedPipelineLayoutDesc->_stageInputs.empty() &&
                (reflectionDataGroup->_vertexInputLayouts.empty() == false))
            {
                std::move(reflectionDataGroup->_vertexInputLayouts.begin(),
                          reflectionDataGroup->_vertexInputLayouts.end(),
                          std::back_inserter(
                              combinedPipelineLayoutDesc->_stageInputs));
            }

            if (combinedPipelineLayoutDesc->_stageOutputs.empty() &&
                (reflectionDataGroup->_fragmentOutputLayouts.empty() == false))
            {
                std::move(reflectionDataGroup->_fragmentOutputLayouts.begin(),
                          reflectionDataGroup->_fragmentOutputLayouts.end(),
                          std::back_inserter(
                              combinedPipelineLayoutDesc->_stageOutputs));
            }
        }
    }
}

bool PipelineLayout::initialize(std::vector<const ShaderReflectionDataGroup*>&&
                                    combinedReflectionDataGroups)
{
    organizeCombinedDescSetLayouts(std::move(combinedReflectionDataGroups),
                                   &_combinedPipelineLayoutDesc);

    DescriptorSetAllocatorPool* descriptorSetAllocatorPool =
        _logicalDevice->getDescriptorSetAllocatorPool();

    std::vector<VkDescriptorSetLayout> vkSetLayouts;

    // Set index 0 is always bindless descriptor set.
    _setAllocators[0] =
        descriptorSetAllocatorPool->getBindlessDescriptorSetAllocator();
    vkSetLayouts.push_back(_setAllocators[0]->getVkDescriptorSetLayout());

    for (uint32_t set = 1; set < MAX_NUM_SET_SLOTS; ++set)
    {
        if (_combinedPipelineLayoutDesc._sets[set]._stageFlags != 0)
        {
            _setAllocators[set] =
                descriptorSetAllocatorPool->getOrCreateDescriptorSetAllocator(
                    _combinedPipelineLayoutDesc._sets[set]);
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