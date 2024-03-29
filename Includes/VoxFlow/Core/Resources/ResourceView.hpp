// Author : snowapril

#ifndef VOXEL_FLOW_BINDABLE_RESOURCE_VIEW_HPP
#define VOXEL_FLOW_BINDABLE_RESOURCE_VIEW_HPP

#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <memory>
#include <string>

namespace VoxFlow
{
class LogicalDevice;
class RenderResource;

enum class ResourceViewType : uint8_t
{
    BufferView = 0,
    ImageView = 1,
    StagingBufferView = 2,
    Undefined = 3,
    Count = Undefined
};

class ResourceView : private NonCopyable
{
 public:
    explicit ResourceView(std::string&& debugName, LogicalDevice* logicalDevice, RenderResource* ownerResource);
    virtual ~ResourceView();

    virtual ResourceViewType getResourceViewType() const = 0;

    [[nodiscard]] inline RenderResource* getOwnerResource() const
    {
        return _ownerResource;
    }

    inline void setLastAccessMask(ResourceAccessMask accessMask)
    {
        _lastAccessMask = accessMask;
    }

    inline void setLastusedShaderStageFlags(VkPipelineStageFlags stageFlags)

    {
        _lastUsedStageFlags = stageFlags;
    }

    [[nodiscard]] inline ResourceAccessMask getLastAccessMask() const
    {
        return _lastAccessMask;
    }

    [[nodiscard]] inline VkPipelineStageFlags getLastusedShaderStageFlags() const
    {
        return _lastUsedStageFlags;
    }

 protected:
    std::string _debugName;
    LogicalDevice* _logicalDevice = nullptr;
    ResourceAccessMask _lastAccessMask = ResourceAccessMask::Undefined;
    VkPipelineStageFlags _lastUsedStageFlags = VK_PIPELINE_STAGE_NONE;
    std::vector<FenceObject> _accessedFences;
    RenderResource* _ownerResource = nullptr;
};
}  // namespace VoxFlow

#endif