// Author : snowapril

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <algorithm>
#include <glm/common.hpp>

namespace VoxFlow
{
DescriptorSetAllocatorPool::DescriptorSetAllocatorPool(
    LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
}

DescriptorSetAllocatorPool::~DescriptorSetAllocatorPool()
{
}

DescriptorSetAllocatorPool::DescriptorSetAllocatorPool(
    DescriptorSetAllocatorPool&& rhs)
{
    operator=(std::move(rhs));
}

DescriptorSetAllocatorPool& DescriptorSetAllocatorPool::operator=(
    DescriptorSetAllocatorPool&& rhs)
{
    if (this != &rhs)
    {
        _descriptorSetAllocators.swap(rhs._descriptorSetAllocators);
    }
    return *this;
}

std::shared_ptr<DescriptorSetAllocator>
DescriptorSetAllocatorPool::getOrCreateDescriptorSetAllocator(
    const DescriptorSetLayoutDesc& descSetLayout)
{
    ContainerType::iterator iter = _descriptorSetAllocators.find(descSetLayout);
    if (iter == _descriptorSetAllocators.end())
    {
        std::shared_ptr<DescriptorSetAllocator> setAllocator =
            std::make_shared<DescriptorSetAllocator>(_logicalDevice,
                                                     descSetLayout);
        _descriptorSetAllocators.emplace(descSetLayout, setAllocator);
        return setAllocator;
    }
    return iter->second;
}

}  // namespace VoxFlow