// Author : snowapril

#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocatorPool.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetConfig.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/MathUtils.hpp>
#include <algorithm>

namespace VoxFlow
{
DescriptorSetAllocatorPool::DescriptorSetAllocatorPool(LogicalDevice* logicalDevice) : _logicalDevice(logicalDevice)
{
    _bindlessSetAllocator = std::make_shared<BindlessDescriptorSetAllocator>(_logicalDevice);

    const bool result = _bindlessSetAllocator->initialize(
            DescriptorSetLayoutDesc {
        ._descriptorInfos = {
            DescriptorInfo{ ._descriptorCategory = DescriptorCategory::CombinedImage,
                            ._arraySize = NUM_BINDLESS_DESCRIPTORS[0],
                            ._binding = 0 },
            DescriptorInfo{ ._descriptorCategory = DescriptorCategory::UniformBuffer,
                            ._arraySize = NUM_BINDLESS_DESCRIPTORS[1],
                            ._binding = 1 },
            DescriptorInfo{ ._descriptorCategory = DescriptorCategory::StorageBuffer,
                            ._arraySize = NUM_BINDLESS_DESCRIPTORS[2],
                            ._binding = 2 },
        },
        ._stageFlags = VK_SHADER_STAGE_ALL
    }, FRAME_BUFFER_COUNT);

    VOX_ASSERT(result, "Failed to initialize bindless descriptor set allocator");
}

DescriptorSetAllocatorPool::~DescriptorSetAllocatorPool()
{
}

DescriptorSetAllocatorPool::DescriptorSetAllocatorPool(DescriptorSetAllocatorPool&& rhs)
{
    operator=(std::move(rhs));
}

DescriptorSetAllocatorPool& DescriptorSetAllocatorPool::operator=(DescriptorSetAllocatorPool&& rhs)
{
    if (this != &rhs)
    {
        _descriptorSetAllocators.swap(rhs._descriptorSetAllocators);
    }
    return *this;
}

std::shared_ptr<DescriptorSetAllocator> DescriptorSetAllocatorPool::getOrCreateDescriptorSetAllocator(const DescriptorSetLayoutDesc& descSetLayout)
{
    std::lock_guard<std::mutex> scopedLock(_mutex);

    ContainerType::iterator iter = _descriptorSetAllocators.find(descSetLayout);
    if (iter == _descriptorSetAllocators.end())
    {
        std::shared_ptr<DescriptorSetAllocator> setAllocator = std::make_shared<PooledDescriptorSetAllocator>(_logicalDevice);

        const bool initResult = setAllocator->initialize(descSetLayout, 16);  // TODO(snowapril) : replace this magic number

        VOX_ASSERT(initResult == true, "Failed to initialize DescriptorSetAllocator");

        _descriptorSetAllocators.emplace(descSetLayout, setAllocator);
        return setAllocator;
    }
    return iter->second;
}

std::shared_ptr<DescriptorSetAllocator> DescriptorSetAllocatorPool::getBindlessDescriptorSetAllocator()
{
    return _bindlessSetAllocator;
}

}  // namespace VoxFlow