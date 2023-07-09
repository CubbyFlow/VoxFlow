// Author : snowapril

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <algorithm>
#include <glm/common.hpp>

namespace VoxFlow
{
DescriptorSetAllocatorPool::DescriptorSetAllocatorPool(
    LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
    _bindlessSetAllocator =
        std::make_shared<BindlessDescriptorSetAllocator>(_logicalDevice);

    // TODO(snowapril) : initialize bindless descriptor set allocator
    // DescriptorSetLayoutDesc bindlessSetLayoutDesc = {
    //     ._bindingMap = { DescriptorSetLayoutDesc::CombinedImage{
    //         ._format = VK_FORMAT_UNDEFINED,
    //         ._binding = 0,
    //         ._arraySize = NUM_BINDLESS_DESCRIPTORS[static_cast<uint32_t>(
    //             BindlessDescriptorBinding::CombinedImage)] },
    //
    // },
    //
    // };
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
            std::make_shared<PooledDescriptorSetAllocator>(_logicalDevice);
        
        const bool initResult = setAllocator->initialize(
            descSetLayout, 16);  // TODO(snowapril) : replace this magic number

        VOX_ASSERT(initResult == true,
                   "Failed to initialize DescriptorSetAllocator");

        _descriptorSetAllocators.emplace(descSetLayout, setAllocator);
        return setAllocator;
    }
    return iter->second;
}

std::shared_ptr<DescriptorSetAllocator>
DescriptorSetAllocatorPool::getBindlessDescriptorSetAllocator()
{
    return _bindlessSetAllocator;
}

}  // namespace VoxFlow