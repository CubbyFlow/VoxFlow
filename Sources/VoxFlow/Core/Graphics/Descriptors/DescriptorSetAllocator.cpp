// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Descriptors/DescriptorSetAllocator.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <type_traits>
#include <utility>

namespace VoxFlow
{

DescriptorSetAllocator::DescriptorSetAllocator(LogicalDevice* logicalDevice, const bool isBindless)
    : _logicalDevice(logicalDevice), _isBindless(isBindless)
{
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

bool DescriptorSetAllocator::initialize(
    const DescriptorSetLayoutDesc& setLayout, const uint32_t numSets)
{
    _setLayoutDesc = setLayout;

    const uint32_t numBindings =
        static_cast<uint32_t>(_setLayoutDesc._descriptorInfos.size());

    std::vector<VkDescriptorSetLayoutBinding> descSetLayoutBindings;
    std::vector<VkDescriptorPoolSize> poolSizes;

    descSetLayoutBindings.reserve(numBindings);
    poolSizes.reserve(numBindings);

    for (std::vector<DescriptorInfo>::const_iterator it =
             _setLayoutDesc._descriptorInfos.begin();
         it != _setLayoutDesc._descriptorInfos.end(); ++it)
    {
        VkDescriptorType descriptorType = VK_DESCRIPTOR_TYPE_MAX_ENUM;
        switch (it->_descriptorCategory)
        {
            case DescriptorCategory::CombinedImage:
                descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                break;
            case DescriptorCategory::UniformBuffer:
                descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                break;
            case DescriptorCategory::StorageBuffer:
                descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                break;
            case DescriptorCategory::Undefined:
                VOX_ASSERT(false, "Unknown descriptor type must not be given");
                continue;
        }

        descSetLayoutBindings.push_back(
            { .binding = it->_binding,
              .descriptorType = descriptorType,
              .descriptorCount = it->_arraySize,
              .stageFlags = _setLayoutDesc._stageFlags,
              .pImmutableSamplers = nullptr });
        poolSizes.push_back(
            { .type = descriptorType, .descriptorCount = it->_arraySize });
    }

    // TODO(snowapril) : sort setLayout descriptorBindings according to binding
    // id

    const VkDescriptorBindingFlags bindingFlag =
        (_isBindless) ? (VK_DESCRIPTOR_BINDING_UPDATE_AFTER_BIND_BIT |
                         VK_DESCRIPTOR_BINDING_PARTIALLY_BOUND_BIT)
                      : 0;
    std::vector<VkDescriptorBindingFlags> bindingFlags(numBindings,
                                                       bindingFlag);

    VkDescriptorSetLayoutBindingFlagsCreateInfo bindingFlagsCreateInfo = {
        .sType =
            VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_BINDING_FLAGS_CREATE_INFO,
        .pNext = nullptr,
        .bindingCount = numBindings,
        .pBindingFlags = bindingFlags.data()
    };

    VkDescriptorSetLayoutCreateInfo createInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
        .pNext = &bindingFlagsCreateInfo,
        .flags =
            _isBindless
                ? VK_DESCRIPTOR_SET_LAYOUT_CREATE_UPDATE_AFTER_BIND_POOL_BIT
                : 0U,
        .bindingCount = static_cast<uint32_t>(numBindings),
        .pBindings = descSetLayoutBindings.data()
    };

    VK_ASSERT(vkCreateDescriptorSetLayout(_logicalDevice->get(), &createInfo,
                                          nullptr, &_vkSetLayout));

    VkDescriptorPoolCreateInfo poolCreateInfo = {
        .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
        .pNext = nullptr,
        .flags =
            _isBindless ? VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT : 0U,
        .maxSets = numSets,
        .poolSizeCount = numBindings,
        .pPoolSizes = poolSizes.data(),
    };
    VK_ASSERT(vkCreateDescriptorPool(_logicalDevice->get(), &poolCreateInfo,
                                     nullptr, &_vkDescPool));

    // Prepare VkDescriptorSets early if bindless
    if (_isBindless)
    {
        std::vector<VkDescriptorSet> vkDescSets(numSets);
        std::vector<VkDescriptorSetLayout> vkDescSetLayouts(numSets,
                                                            _vkSetLayout);
        VkDescriptorSetAllocateInfo allocInfo = {
            .sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
            .pNext = nullptr,
            .descriptorPool = _vkDescPool,
            .descriptorSetCount = numSets,
            .pSetLayouts = vkDescSetLayouts.data()
        };
        vkAllocateDescriptorSets(_logicalDevice->get(), &allocInfo,
                                 vkDescSets.data());

        for (VkDescriptorSet set : vkDescSets)
        {
            _descriptorSetNodes.push_back(
                { ._vkDescriptorSet = set,
                  ._lastAccessedFenceObject = FenceObject::Default() });
        }
    }

    return true;
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

PooledDescriptorSetAllocator::PooledDescriptorSetAllocator(
    LogicalDevice* logicalDevice)
    : DescriptorSetAllocator(logicalDevice, false)
{
}

PooledDescriptorSetAllocator::~PooledDescriptorSetAllocator()
{
}

PooledDescriptorSetAllocator::PooledDescriptorSetAllocator(
    PooledDescriptorSetAllocator&& other) noexcept
    : DescriptorSetAllocator(std::move(other))
{
    operator=(std::move(other));
}

PooledDescriptorSetAllocator& PooledDescriptorSetAllocator::operator=(
    PooledDescriptorSetAllocator&& other) noexcept
{
    DescriptorSetAllocator::operator=(std::move(other));
    return *this;
}

VkDescriptorSet PooledDescriptorSetAllocator::getOrCreatePooledDescriptorSet(
    const FenceObject& fenceObject)
{
    VkDescriptorSet vkPooledDescriptorSet = VK_NULL_HANDLE;
    for (DescriptorSetNode& node : _descriptorSetNodes)
    {
        if (node._lastAccessedFenceObject.isCompleted())
        {
            node._lastAccessedFenceObject = fenceObject;
            vkPooledDescriptorSet = node._vkDescriptorSet;
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

        _descriptorSetNodes.push_back(
            { ._vkDescriptorSet = vkPooledDescriptorSet,
              ._lastAccessedFenceObject = fenceObject });
    }

    return vkPooledDescriptorSet;
}

BindlessDescriptorSetAllocator::BindlessDescriptorSetAllocator(
    LogicalDevice* logicalDevice)
    : DescriptorSetAllocator(logicalDevice, true)
{
}

BindlessDescriptorSetAllocator::~BindlessDescriptorSetAllocator()
{
}

BindlessDescriptorSetAllocator::BindlessDescriptorSetAllocator(
    BindlessDescriptorSetAllocator&& other) noexcept
    : DescriptorSetAllocator(std::move(other))
{
    operator=(std::move(other));
}

BindlessDescriptorSetAllocator& BindlessDescriptorSetAllocator::operator=(
    BindlessDescriptorSetAllocator&& other) noexcept
{
    DescriptorSetAllocator::operator=(std::move(other));
    return *this;
}

VkDescriptorSet BindlessDescriptorSetAllocator::getBindlessDescriptorSet(
    const uint32_t setIndex, const FenceObject& fenceObject)
{
    if (setIndex >= static_cast<uint32_t>(_descriptorSetNodes.size()))
    {
        VOX_ASSERT(false, "setIndex must be under {}",
                   _descriptorSetNodes.size());
        return VK_NULL_HANDLE;
    }
    DescriptorSetNode& setNode = _descriptorSetNodes[setIndex];

    setNode._lastAccessedFenceObject = fenceObject;
    // TODO(snowapril) : avoid synchronization issue if different queue request
    // same set

    return setNode._vkDescriptorSet;
}

}  // namespace VoxFlow