// Author : snowapril

#ifndef VOXEL_FLOW_RESOURCE_TRACKER_HPP
#define VOXEL_FLOW_RESOURCE_TRACKER_HPP

#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Resources/Handle.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string_view>
#include <unordered_map>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;

class ResourceTracker : private NonCopyable
{
 public:
    static ResourceTracker& This()
    {
        static ResourceTracker sResourceTracker;
        return sResourceTracker;
    }

    void release()
    {
        _renderResources.clear();
    }

 public:
    template <typename RenderResourceType>
    Handle<RenderResourceType> allocate(std::string_view&& debugName,
                                        LogicalDevice* logicalDevice,
                                        RenderResourceMemoryPool* memoryPool)
    {
        std::unique_ptr<RenderResource> renderResource =
            std::make_unique<RenderResourceType>(std::move(debugName),
                                                 logicalDevice, memoryPool);

        Handle resourceHandle(renderResource);
        HandleBase::HandleID handleID = resourceHandle.getHandleID();

        auto iter = _renderResources.find(handleID);
        if (iter != _renderResources.end())
        {
            VOX_ASSERT(false, "Duplicate HandleID must not be exist");
        }
        else
        {
            _renderResources.insert(handleID, std::move(renderResource));
        }

        return std::move(resourceHandle);
    }

    void deallocate(HandleBase::HandleID handleID)
    {
        // TODO(snowapril) : refCount manage
        auto iter = _renderResources.find(handleID);
        if (iter != _renderResources.end())
        {
            _renderResources.erase(iter);
        }
        else
        {
            VOX_ASSERT(false, "Unknown handleID must not be inserted");
        }
    }

    void setFenceObject(HandleBase::HandleID handleID, FenceObject fenceObject)
    {
        // TODO(snowapril) : need simultaneous access tracking implement
        (void)handleID;
        (void)fenceObject;
    }

 private:
    std::unordered_map<HandleBase::HandleID, std::unique_ptr<RenderResource>>
        _renderResources;

 private:
    ResourceTracker(){};
    ~ResourceTracker()
    {
        release();
    };
    ResourceTracker(ResourceTracker&& rhs)
    {
        operator=(std::move(rhs));
    }
    ResourceTracker& operator=(ResourceTracker&& rhs)
    {
        if (this != &rhs)
        {
            _renderResources.swap(rhs._renderResources);
        }
        return *this;
    }
};

}  // namespace VoxFlow

#endif