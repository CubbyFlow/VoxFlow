// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_RESOURCE_GARBAGE_COLLECTOR_HPP
#define VOXEL_FLOW_RENDER_RESOURCE_GARBAGE_COLLECTOR_HPP

#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/Thread.hpp>
#include <functional>
#include <mutex>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;

struct RenderResourceGarbage
{
    std::vector<FenceObject> _accessedFences;
    std::function<void()> _deletionDelegate;

    RenderResourceGarbage() = default;
    RenderResourceGarbage(std::vector<FenceObject>&& accessedFences,
                          std::function<void()>&& deletionDelegate)
        : _accessedFences(std::move(accessedFences)),
          _deletionDelegate(std::move(deletionDelegate))
    {
    }
    ~RenderResourceGarbage() = default;
    RenderResourceGarbage(const RenderResourceGarbage& rhs)
    {
        operator=(rhs);
    }
    RenderResourceGarbage(RenderResourceGarbage&& rhs)
    {
        operator=(std::move(rhs));
    }
    RenderResourceGarbage& operator=(const RenderResourceGarbage& rhs)
    {
        if (this != &rhs)
        {
            _accessedFences = rhs._accessedFences;
            _deletionDelegate = rhs._deletionDelegate;
        }
        return *this;
    }
    RenderResourceGarbage& operator=(RenderResourceGarbage&& rhs)
    {
        if (this != &rhs)
        {
            _accessedFences.swap(rhs._accessedFences);
            _deletionDelegate.swap(rhs._deletionDelegate);
        }
        return *this;
    }
};

class RenderResourceGarbageCollector : private Thread
{
 public:
    static RenderResourceGarbageCollector& Get();

    // Push render resource garbage (buffer, texture, or etc..) to the queue
    void pushRenderResourceGarbage(RenderResourceGarbage&& garbage);

    // Process collected garbages deletion if possible
    void processRenderResourceGarbage();

 private:
    RenderResourceGarbageCollector() = default;
    ~RenderResourceGarbageCollector() = default;

 public:
    void threadConstruct() override final;
    void threadProcess() override final;
    void threadTerminate() override final;

 public:
    std::mutex _garbageCollectionLock;
    std::vector<RenderResourceGarbage> _garbageCollection;
};
}  // namespace VoxFlow

#endif