// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_RESOURCE_HPP
#define VOXEL_FLOW_RENDER_RESOURCE_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <string>

namespace VoxFlow
{
class LogicalDevice;

enum class RenderResourceType : uint8_t
{
    Buffer = 0,
    Texture = 1,
    Undefined = 2,
    Count = Undefined
};

class RenderResource : private NonCopyable
{
 public:
    RenderResource(std::string&& debugName, LogicalDevice* logicalDevice,
                   RenderResourceMemoryPool* renderResourceMemoryPool);
    virtual ~RenderResource(){};
    RenderResource(RenderResource&&) = default;
    RenderResource& operator=(RenderResource&&) = default;

 public:
    virtual RenderResourceType getResourceType() const;

 protected:
    std::string _debugName;
    LogicalDevice* _logicalDevice = nullptr;
    RenderResourceMemoryPool* _renderResourceMemoryPool = nullptr;
    VmaAllocation _allocation = nullptr;

    std::vector<FenceObject> _accessedFences;

    void* _permanentMappedAddress = nullptr;
};

}  // namespace VoxFlow

#endif