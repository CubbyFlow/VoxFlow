// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <utility>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <type_traits>

namespace VoxFlow
{

DescriptorSetAllocator::DescriptorSetAllocator(
    LogicalDevice* logicalDevice, const DescriptorSetLayoutDesc& setLayout)
    : _logicalDevice(logicalDevice), _setLayoutDesc(setLayout)
{
    const uint32_t numBindings =
        static_cast<uint32_t>(_setLayoutDesc._bindingMap.size());

    std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings;
    std::vector<VkDescriptorPoolSize> poolSizes;

    descSetLayoutBindings.reserve(numBindings);
    poolSizes.reserve(numBindings);

    for (DescriptorSetLayoutDesc::ContainerType::const_iterator it =
             _setLayoutDesc._bindingMap.begin();
         it != _setLayoutDesc._bindingMap.end(); ++it)
    {
        std::visit(
            overloaded{
                [this, &descSetLayoutBindings, &poolSizes](
                    DescriptorSetLayoutDesc::CombinedImage setBinding) {
                    descSetLayoutBindings.push_back(
                        { .binding = setBinding._binding,
                          .descriptorType =
                              VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          .descriptorCount = setBinding._arraySize,
                          .stageFlags = _setLayoutDesc._stageFlags,
                          .pImmutableSamplers = nullptr });
                    poolSizes.push_back(
                        { .type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
                          .descriptorCount = setBinding._arraySize });
                },
                [this, &descSetLayoutBindings, &poolSizes](
                    DescriptorSetLayoutDesc::UniformBuffer setBinding) {
                    descSetLayoutBindings.push_back(
                        { .binding = setBinding._binding,
                          .descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                          .descriptorCount = setBinding._arraySize,
                          .stageFlags = _setLayoutDesc._stageFlags,
                          .pImmutableSamplers = nullptr });
                    poolSizes.push_back(
                        { .type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
                          .descriptorCount = setBinding._arraySize });
                },
                [this, &descSetLayoutBindings, &poolSizes](
                    DescriptorSetLayoutDesc::StorageBuffer setBinding) {
                    descSetLayoutBindings.push_back(
                        { .binding = setBinding._binding,
                          .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                          .descriptorCount = setBinding._arraySize,
                          .stageFlags = _setLayoutDesc._stageFlags,
                          .pImmutableSamplers = nullptr });
                    poolSizes.push_back(
                        { .type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                          .descriptorCount = setBinding._arraySize });
                },
            },
            it->second);
    }


    // TODO(snowapril) : sort setLayout descriptorBindings according to binding
    // id
    VkDescriptorSetLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .bindingCount = static_cast<uint32_t>(numBindings),
        .pBindings = descSetLayoutBindings.data()
    };

    VK_ASSERT(vkCreateDescriptorSetLayout(_logicalDevice->get(), &createInfo,
                                          nullptr, &_vkSetLayout));

    VkDescriptorPoolCreateInfo poolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags = 0,
        .maxSets = MAX_NUM_DESC_SETS,
        .poolSizeCount = numBindings,
        .pPoolSizes = poolSizes.data(),
    };
    VK_ASSERT(vkCreateDescriptorPool(_logicalDevice->get(), &poolCreateInfo,
                                     nullptr, &_vkDescPool));
}

DescriptorSetAllocator::~DescriptorSetAllocator()
{
    release();
}

DescriptorSetAllocator::DescriptorSetAllocator(
    DescriptorSetAllocator&& other) noexcept
{
    operator=(std::move(other));
}

DescriptorSetAllocator& DescriptorSetAllocator::operator=(
    DescriptorSetAllocator&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = other._logicalDevice;
        _setLayoutDesc = other._setLayoutDesc;
        _vkSetLayout = other._vkSetLayout;
    }
    return *this;
}

VkDescriptorSet DescriptorSetAllocator::getOrCreatePooledDescriptorSet(
    const FenceObject& fenceObject)
{
    VkDescriptorSet vkPooledDescriptorSet = VK_NULL_HANDLE;
    for (PooledDescriptorSetNode& node : _pooledDescriptorSetNodes)
    {
        if (node._lastAccessedFenceObject.isCompleted())
        {
            node._lastAccessedFenceObject = fenceObject;
            vkPooledDescriptorSet = node._vkPooledDescriptorSet;
        }
    }

    if (vkPooledDescriptorSet == VK_NULL_HANDLE)
    {
        VkDescriptorSetAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = 0,
            .descriptorPool = _vkDescPool,
            .descriptorSetCount = 1,
            .pSetLayouts = &_vkSetLayout,
        };
        VK_ASSERT(vkAllocateDescriptorSets(_logicalDevice->get(), &allocInfo,
                                           &vkPooledDescriptorSet));

        _pooledDescriptorSetNodes.push_back(
            { vkPooledDescriptorSet, fenceObject });
    }

    return vkPooledDescriptorSet;
}

VkDescriptorSet DescriptorSetAllocator::allocateBindlessDescriptorSet()
{
    // TODO(snowapril) : fill implementation
    return VK_NULL_HANDLE;
}

void DescriptorSetAllocator::release()
{
    if (_vkDescPool)
    {
        vkDestroyDescriptorPool(_logicalDevice->get(), _vkDescPool, nullptr);
    }

    if (_vkSetLayout)
    {
        vkDestroyDescriptorSetLayout(_logicalDevice->get(), _vkSetLayout,
                                     nullptr);
    }
}

}  // namespace VoxFlow