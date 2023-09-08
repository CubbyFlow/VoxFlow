// Author : snowapril

#ifndef VOXEL_FLOW_BINDABLE_RESOURCE_VIEW_HPP
#define VOXEL_FLOW_BINDABLE_RESOURCE_VIEW_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <string>
#include <memory>

namespace VoxFlow
{
class LogicalDevice;
class RenderResource;

enum class ResourceViewType : uint8_t
{
    BufferView = 0,
    ImageView = 1,
};

class BindableResourceView : private NonCopyable
{
 public:
    explicit BindableResourceView(
        std::string&& debugName, LogicalDevice* logicalDevice,
                                  RenderResource* ownerResource);
    virtual ~BindableResourceView();

    virtual ResourceViewType getResourceViewType() const = 0;

    [[nodiscard]] inline RenderResource* getOwnerResource()
    {
        return _ownerResource;
    }

 protected:
    std::string _debugName;
    LogicalDevice* _logicalDevice = nullptr;
    ResourceAccessMask _lastAccessMask = ResourceAccessMask::Undefined;
    std::vector<FenceObject> _accessedFences;
    RenderResource* _ownerResource = nullptr;
};
}  // namespace VoxFlow

#endif