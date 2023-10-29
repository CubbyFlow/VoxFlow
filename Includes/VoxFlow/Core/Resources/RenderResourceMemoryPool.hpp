// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_RESOURCE_MEMORY_POOL_HPP
#define VOXEL_FLOW_RENDER_RESOURCE_MEMORY_POOL_HPP

#include <volk/volk.h>
#include <vma/include/vk_mem_alloc.h>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;
class PhysicalDevice;
class Instance;

class RenderResourceMemoryPool : private NonCopyable
{
 public:
    explicit RenderResourceMemoryPool(LogicalDevice* logicalDevice, PhysicalDevice* physicalDevice, Instance* instance);
    ~RenderResourceMemoryPool();

 public:
    [[nodiscard]] inline VmaAllocator get() const
    {
        return _allocator;
    }

    bool initialize();
    void release();

 protected:
 private:
    LogicalDevice* _logicalDevice = nullptr;
    PhysicalDevice* _physicalDevice = nullptr;
    Instance* _instance = nullptr;
    VmaAllocator _allocator = nullptr;
};
}  // namespace VoxFlow

#endif