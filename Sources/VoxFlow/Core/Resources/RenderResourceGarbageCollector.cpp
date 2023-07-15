// Author : snowapril

#include <VoxFlow/Core/Resources/RenderResourceGarbageCollector.hpp>
#include <VoxFlow/Core/Utils/ChromeTracer.hpp>
#include <algorithm>

namespace VoxFlow
{

RenderResourceGarbageCollector& RenderResourceGarbageCollector::Get()
{
    static RenderResourceGarbageCollector sGarbageCollectorInstance;
    return sGarbageCollectorInstance;
}

void RenderResourceGarbageCollector::pushRenderResourceGarbage(
    RenderResourceGarbage&& garbage)
{
    std::lock_guard<std::mutex> scopedLock(_garbageCollectionLock);
    _garbageCollection.emplace_back(std::move(garbage));
}

void RenderResourceGarbageCollector::processRenderResourceGarbage()
{
    SCOPED_CHROME_TRACING("RenderResourceGarbageCollector::processRenderResourceGarbage");

    static std::vector<RenderResourceGarbage> sTmpGarbageCollection;

    {
        std::lock_guard<std::mutex> scopedLock(_garbageCollectionLock);
        sTmpGarbageCollection.reserve(_garbageCollection.size() +
                                      sTmpGarbageCollection.size());
        sTmpGarbageCollection.insert(
            sTmpGarbageCollection.end(),
            std::make_move_iterator(_garbageCollection.begin()),
            std::make_move_iterator(_garbageCollection.end()));
    }

    uint32_t numGarbages = static_cast<uint32_t>(sTmpGarbageCollection.size());
    uint32_t numDeletedResources = 0;
    for (uint32_t i = 0; i < numGarbages;)
    {
        RenderResourceGarbage& resourceGarbage = sTmpGarbageCollection[i];

        bool isUsingResource = false;
        for (const FenceObject& accessedFence : resourceGarbage._accessedFences)
        {
            isUsingResource |= (accessedFence.isCompleted() == false);
            if (isUsingResource)
            {
                break;
            }
        }

        if (isUsingResource == false)
        {
            std::invoke(resourceGarbage._deletionDelegate);
            std::swap(resourceGarbage, sTmpGarbageCollection[numGarbages - 1]);
            ++numDeletedResources;
            --numGarbages;
        }
        else
        {
            ++i;
        }
    }

    sTmpGarbageCollection.resize(sTmpGarbageCollection.size() -
                                 numDeletedResources);
}

void RenderResourceGarbageCollector::threadConstruct()
{
    _isConstructed = true;
    _shouldTerminate = false;

    _threadHandle =
        std::thread(&RenderResourceGarbageCollector::threadProcess, this);
}

void RenderResourceGarbageCollector::threadProcess()
{
    using namespace std::chrono_literals;
    while (true)
    {
        processRenderResourceGarbage();

        if (_shouldTerminate)
        {
            break;
        }

        std::this_thread::sleep_for(1000ms);
    }
}

void RenderResourceGarbageCollector::threadTerminate()
{
    _shouldTerminate = true;
    _isConstructed = false;

    if (_threadHandle.joinable())
    {
        _threadHandle.join();
    }
}

}  // namespace VoxFlow