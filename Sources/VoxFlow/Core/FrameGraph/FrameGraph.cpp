// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <stack>
#include "VoxFlow/Core/Utils/ChromeTracer.hpp"
#include "VoxFlow/Core/Utils/Logger.hpp"

namespace VoxFlow
{

namespace FrameGraph
{
ResourceHandle FrameGraphBuilder::read(ResourceHandle id)
{
    return _frameGraph->readInternal(id, _currentPassNode);
}

ResourceHandle FrameGraphBuilder::write(ResourceHandle id)
{
    return _frameGraph->writeInternal(id, _currentPassNode);
}

FrameGraph::FrameGraph()
{
}

FrameGraph::~FrameGraph()
{
}

void FrameGraph::reset(CommandExecutorBase* commandExecutor,
                       RenderResourceAllocator* renderResourceAllocator)
{
    clear();

    _commandExecutor = commandExecutor;
    _renderResourceAllocator = renderResourceAllocator;
}

bool FrameGraph::compile()
{
    SCOPED_CHROME_TRACING("FrameGraph::compile");

    const uint32_t numPassNodes = static_cast<uint32_t>(_passNodes.size());
    if (numPassNodes == 0)
    {
        return false;
    }

#define FRAMEGRAPH_FULL_OPTIMIZE
    //#define FRAMEGRAPH_CULLING_ONLY
#if defined(FRAMEGRAPH_FULL_OPTIMIZE)
    buildAdjacencyLists(numPassNodes);

    if (bool isCycleExist = topologicalSortPassNodes(numPassNodes))
    {
        VOX_ASSERT(isCycleExist == false, "The cyclic dependency is detected");
        return false;
    }

    // calcDependencyLevels(numPassNodes);

    // allotCommandQueueIndices(numPassNodes);
#elif defined(FRAMEGRAPH_CULLING_ONLY)
    for (uint32_t i = 0; i < numPassNodes; ++i)
    {
        PassNode& passNode = _passNodes[i];
        const std::vector<ResourceHandle>& readResources =
            passNode.getReadResources();
        const std::vector<ResourceHandle>& writeResources =
            passNode.getWriteResources();

        passNode.addRefCount(static_cast<uint32_t>(writeResources.size()));
        for (ResourceHandle readResourceHandle : readResources)
        {
            _resources[readResourceHandle].addRefCount(1);
        }
    }

    const uint32_t numResourceRegistries =
        static_cast<uint32_t>(_resources.size());

    std::stack<ResourceHandle> unusedResources;
    for (uint32_t i = 0; i < numResourceRegistries; ++i)
    {
        if (_resources[i].getRefCount() == 0)
        {
            unusedResources.push(ResourceHandle(i));
        }
    }

    while (unusedResources.empty() == false)
    {
        ResourceHandle resourceHandle = unusedResources.top();
        unusedResources.pop();

        FrameGraphResourceRegistry& resourceRegistry =
            _resources[resourceHandle];

        PassNode* producer = resourceRegistry->getProducerNode();
        if (producer->subRefCount(1) == 0)
        {
            for (const ResourceHandle& readResourceHandle :
                 producer->getReadResources())
            {
                FrameGraphResourceRegistry& readResourceRegistry =
                    _resources[readResourceHandle];

                if (readResourceRegistry.subRefCount(1) == 0)
                {
                    unusedResources.push(readResourceHandle);
                }
            }
        }
    }
#else

#endif

    return true;
}


ResourceHandle FrameGraph::readInternal(ResourceHandle id, PassNode* passNode)
{
    VOX_ASSERT(id < static_cast<ResourceHandle>(_resourceSlots.size()),
               "Invalid ResourceHandle({}) is given",
               static_cast<uint32_t>(id));

    const ResourceSlot& resourceSlot = getResourceSlot(id);
    VirtualResource* resource = _resources[resourceSlot._resourceIndex];
    ResourceNode* resourceNode = _resourceNodes[resourceSlot._nodeIndex];

    DependencyGraph::EdgeContainer incomingEdges =
        _dependencyGraph.getIncomingEdges(resourceNode->getNodeID());

    _dependencyGraph.link(passNode->getNodeID(), resourceNode->getNodeID());

    return id;
}

ResourceHandle FrameGraph::writeInternal(ResourceHandle id, PassNode* passNode)
{
    VOX_ASSERT(id < static_cast<ResourceHandle>(_resourceSlots.size()),
               "Invalid ResourceHandle({}) is given",
               static_cast<uint32_t>(id));

    const ResourceSlot& resourceSlot = getResourceSlot(id);
    VirtualResource* resource = _resources[resourceSlot._resourceIndex];
    ResourceNode* resourceNode = _resourceNodes[resourceSlot._nodeIndex];

    DependencyGraph::EdgeContainer outgoingEdges =
        _dependencyGraph.getOutgoingEdges(resourceNode->getNodeID());

    _dependencyGraph.link(resourceNode->getNodeID(), passNode->getNodeID());

    return id;
}

void FrameGraph::buildAdjacencyLists(const uint32_t numPassNodes)
{
    SCOPED_CHROME_TRACING("FrameGraph::buildAdjacencyLists");

    _passNodeAdjacencyList.resize(numPassNodes);

    // Build pass node adjacency list
    for (uint32_t i = 0; i < numPassNodes; ++i)
    {
        std::vector<uint32_t>& singleAdjacencyList = _passNodeAdjacencyList[i];

        for (uint32_t j = 0; j < numPassNodes; ++j)
        {
            if (i == j)
                continue;

            for (ResourceHandle writeResource :
                 _passNodes[i]->getWriteResources())
            {
                const std::vector<ResourceHandle>& readResources =
                    _passNodes[j]->getReadResources();

                std::vector<ResourceHandle>::const_iterator iter =
                    std::find(readResources.begin(), readResources.end(),
                              writeResource);

                if (iter != readResources.end())
                {
                    singleAdjacencyList.emplace_back(j);
                }
            }
        }
    }
}

bool FrameGraph::topologicalSortPassNodes(const uint32_t numPassNodes)
{
    SCOPED_CHROME_TRACING("FrameGraph::topologicalSortPassNodes");

    _topologicalSortedPassNodes.reserve(numPassNodes);

    std::vector<bool> permanentMarks(numPassNodes, false);
    std::vector<bool> temporaryMarks(numPassNodes, false);

    bool isCycleExist = false;
    std::function<void(uint32_t)> dfs = [&](uint32_t nodeIndex) {
        if (permanentMarks[nodeIndex])
        {
            return;
        }

        if (temporaryMarks[nodeIndex])
        {
            isCycleExist = true;
            return;
        }

        temporaryMarks[nodeIndex] = true;

        for (uint32_t connectedNode : _passNodeAdjacencyList[nodeIndex])
        {
            dfs(connectedNode);
        }

        temporaryMarks[nodeIndex] = false;
        permanentMarks[nodeIndex] = true;
        _topologicalSortedPassNodes.emplace_back(nodeIndex);
    };

    for (uint32_t i = 0; i < numPassNodes; ++i)
    {
        if (permanentMarks[i] == false)
        {
            dfs(i);

            if (isCycleExist)
            {
                break;
            }
        }
    }

    return isCycleExist;
}

void FrameGraph::calcDependencyLevels(const uint32_t numPassNodes)
{
    SCOPED_CHROME_TRACING("FrameGraph::calcDependencyLevels");

    _dependencyLevels.resize(numPassNodes);

    static std::vector<int32_t> sDistances;
    sDistances.resize(numPassNodes);
    std::fill(sDistances.begin(), sDistances.end(), INT32_MIN);

    const uint32_t startNodeIndex = _topologicalSortedPassNodes.front();
    sDistances[startNodeIndex] = 0;

    int32_t maxDistances = 0;
    for (uint32_t nodeIndex : _topologicalSortedPassNodes)
    {
        for (uint32_t connectedNodeIndex : _passNodeAdjacencyList[nodeIndex])
        {
            if (sDistances[connectedNodeIndex] < sDistances[nodeIndex] + 1)
            {
                sDistances[connectedNodeIndex] = sDistances[nodeIndex] + 1;
                maxDistances =
                    std::max(maxDistances, sDistances[connectedNodeIndex]);
            }
        }
    }

    _dependencyLevels.resize(maxDistances);
    for (uint32_t nodeIndex : _topologicalSortedPassNodes)
    {
        const uint32_t distance = sDistances[nodeIndex];
        _dependencyLevels[distance].push_back(nodeIndex);
    }
}

void FrameGraph::allotCommandQueueIndices(const uint32_t numPassNodes)
{
    SCOPED_CHROME_TRACING("FrameGraph::allotCommandQueueIndices");
    // for (uint32_t nodeIndex : _topologicalSortedPassNodes)
    // {
    //     _commandQueueIndices
    // }

    // TODO(snowapril) : at now, only consider single queue.
    // As resource transition from one queue to the other queue should be
    // accompanied by queue ownership transfer, there are plenty of things
    // to consider for minimizing synchronizations

    _commandQueueIndices.resize(numPassNodes, 0);
}

void FrameGraph::calcResourceLifetimes(const uint32_t numPassNodes)
{
    (void)numPassNodes;
}

void FrameGraph::buildSSIS(const uint32_t numPassNodes)
{
    SCOPED_CHROME_TRACING("FrameGraph::buildSSIS");
    (void)numPassNodes;
}

void FrameGraph::execute()
{
    SCOPED_CHROME_TRACING("FrameGraph::execute");

    for (const uint32_t passNodeIndex : _topologicalSortedPassNodes)
    {
        PassNode* passNode = _passNodes[passNodeIndex];

        // TODO(snowapril) : make resources resident

        passNode->execute(this, _commandExecutor);
    }
}

void FrameGraph::clear()
{
    // TODO(snowapril) : clear and re-record only modified pass.
    _passNodes.clear();
    _resourceNodes.clear();
    _resources.clear();
    _passNodeAdjacencyList.clear();
    _topologicalSortedPassNodes.clear();
    _dependencyLevels.clear();
    _commandQueueIndices.clear();
}

void FrameGraph::dumpGraphViz(std::ostringstream& osstr)
{
    (void)osstr;
}
}  // namespace FrameGraph

}  // namespace VoxFlow