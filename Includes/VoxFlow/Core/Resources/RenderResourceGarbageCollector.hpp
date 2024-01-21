// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_RESOURCE_GARBAGE_COLLECTOR_HPP
#define VOXEL_FLOW_RENDER_RESOURCE_GARBAGE_COLLECTOR_HPP

#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/Thread.hpp>
#include <functional>
#include <mutex>
#include <vector>

namespace VoxFlow
{
struct RenderResourceGarbage
{
    std::vector<FenceObject> accessedFences;
    std::function<void()> deletionDelegate;
};

class RenderResourceGarbageCollector : private Thread
{
 public:
    // Push render resource garbage (buffer, texture, or etc..) to the queue
    void dispose(RenderResourceGarbage&& garbage);

    // Get global instance of render resource garbage collector
    static RenderResourceGarbageCollector& Get();

 private:
    // Process collected garbages deletion if possible
    void processRenderResourceGarbage();

 public:
    void threadConstruct() final;
    void threadProcess() final;
    void threadTerminate() final;

 private:
    std::mutex _garbageCollectionLock;
    std::vector<RenderResourceGarbage> _garbageCollection;
};
}  // namespace VoxFlow

#endif