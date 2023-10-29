// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_RESOURCE_HPP
#define VOXEL_FLOW_RENDER_RESOURCE_HPP

#include <vma/include/vk_mem_alloc.h>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string>
#include <string_view>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;

enum class RenderResourceType : uint8_t
{
    Buffer = 0,
    Texture = 1,
    StagingBuffer = 2,
    Sampler = 3,
    Undefined = 4,
    Count = Undefined
};

class RenderResource : private NonCopyable
{
 public:
    RenderResource(std::string_view&& debugName, LogicalDevice* logicalDevice, RenderResourceMemoryPool* renderResourceMemoryPool);
    virtual ~RenderResource(){};
    RenderResource(RenderResource&&) = default;
    RenderResource& operator=(RenderResource&&) = default;

    [[nodiscard]] inline LogicalDeviceType getDeviceType() const
    {
        return _deviceType;
    }

    [[nodiscard]] inline uint32_t getCurrentQueueFamilyIndex() const
    {
        return _currentQueueFamilyIndex;
    }

 public:
    virtual RenderResourceType getResourceType() const = 0;

 protected:
    std::string _debugName;
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool = nullptr;
    VmaAllocation _allocation = nullptr;
    std::vector<FenceObject> _accessedFences;
    void* _permanentMappedAddress = nullptr;
    LogicalDeviceType _deviceType = LogicalDeviceType::Undefined;
    uint32_t _currentQueueFamilyIndex = 0;
};

}  // namespace VoxFlow

#endif