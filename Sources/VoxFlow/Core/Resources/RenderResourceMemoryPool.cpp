// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/PhysicalDevice.hpp>
#include <VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp>

namespace VoxFlow
{

RenderResourceMemoryPool::RenderResourceMemoryPool(
    LogicalDevice* logicalDevice, PhysicalDevice* physicalDevice,
    Instance* instance)
    : _logicalDevice(logicalDevice),
      _physicalDevice(physicalDevice),
      _instance(instance)
{
}

RenderResourceMemoryPool::~RenderResourceMemoryPool()
{
    release();
}

bool RenderResourceMemoryPool::initialize()
{
    // TODO(snowapril) : prepare multiple allocators according to resource type
    VmaAllocatorCreateInfo allocatorInfo{
        .flags = 0,
        .physicalDevice = _physicalDevice->get(),
        .device = _logicalDevice->get(),
        .preferredLargeHeapBlockSize = 0, // TODO(snowapril) : customize fields
        .pAllocationCallbacks = nullptr,
        .pDeviceMemoryCallbacks = nullptr,
        .pHeapSizeLimit = nullptr, // TODO(snowapril) : customize fields
        .pVulkanFunctions = nullptr,
        .instance = _instance->get(),
        .vulkanApiVersion = VK_API_VERSION_1_3, // TODO(snowapril) : customize fields
        .pTypeExternalMemoryHandleTypes = nullptr,
    };

    VK_ASSERT(vmaCreateAllocator(&allocatorInfo, &_allocator));
    return true;
}

void RenderResourceMemoryPool::release()
{
    if (_allocator != nullptr)
    {
        vmaDestroyAllocator(_allocator);
    }
}

}  // namespace VoxFlow