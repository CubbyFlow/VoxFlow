// Author : snowapril

#ifndef VOXEL_FLOW_BINDABLE_RESOURCE_VIEW_HPP
#define VOXEL_FLOW_BINDABLE_RESOURCE_VIEW_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <string>

namespace VoxFlow
{
class LogicalDevice;

enum class ResourceViewType : uint8_t
{
    BufferView = 0,
    ImageView = 1,
};

class BindableResourceView : private NonCopyable
{
 public:
    explicit BindableResourceView(std::string&& debugName,
                                  LogicalDevice* logicalDevice);
    virtual ~BindableResourceView();

    virtual ResourceViewType getResourceViewType() const = 0;

 protected:
    std::string _debugName;
    LogicalDevice* _logicalDevice = nullptr;
    ResourceLayout _lastLayout = ResourceLayout::Undefined;
    std::vector<FenceObject> _accessedFences;
};
}  // namespace VoxFlow

#endif