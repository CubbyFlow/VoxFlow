// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphResources.hpp>
#include <VoxFlow/Core/Utils/ChromeTracer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <map>
#include <stack>

namespace VoxFlow
{

namespace RenderGraph
{
uint32_t FrameGraphBuilder::declareRenderPass(std::string_view&& passName, typename FrameGraphRenderPass::Descriptor&& initArgs)
{
    return static_cast<RenderPassNode*>(_currentPassNode)->declareRenderPass(_frameGraph, this, std::move(passName), std::move(initArgs));
}

FrameGraph::FrameGraph()
{
}

FrameGraph::~FrameGraph()
{
}

void FrameGraph::reset(CommandStream* cmdStream, RenderResourceAllocator* renderResourceAllocator)
{
    clear();

    _cmdStream = cmdStream;
    _renderResourceAllocator = renderResourceAllocator;
}

ResourceHandle FrameGraph::importRenderTarget(std::string&& resourceName, FrameGraphTexture::Descriptor&& resourceDescArgs,
                                              typename FrameGraphRenderPass::ImportedDescriptor&& importedDesc, TextureView* textureView)
{
    VirtualResource* virtualResource = new ImportedRenderTarget(std::move(resourceName), std::move(resourceDescArgs), std::move(importedDesc), {}, textureView);

    ResourceHandle resourceHandle(_resources.size());

    _resourceSlots.push_back({ ._resourceIndex = static_cast<ResourceSlot::IndexType>(_resourceNodes.size()),
                               ._nodeIndex = static_cast<ResourceSlot::IndexType>(_resources.size()),
                               ._version = static_cast<ResourceSlot::VersionType>(0) });
    _resources.push_back(virtualResource);

    ResourceNode* resourceNode = new ResourceNode(&_dependencyGraph, resourceHandle);
    resourceNode->_refCount = UINT32_MAX;

    _resourceNodes.push_back(resourceNode);

    return resourceHandle;
}

bool FrameGraph::compile()
{
    SCOPED_CHROME_TRACING("FrameGraph::compile");

    const uint32_t numPassNodes = static_cast<uint32_t>(_passNodes.size());
    if (numPassNodes == 0)
    {
        return false;
    }

// #define FRAMEGRAPH_FULL_OPTIMIZE
#define FRAMEGRAPH_CULLING_ONLY
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

    _dependencyGraph.cullUnreferencedNodes();

    _passNodeLast = std::stable_partition(_passNodes.begin(), _passNodes.end(), [](PassNode* node) { return node->isCulled() == false; });

    for (auto it = _passNodes.begin(); it != _passNodeLast; ++it)
    {
        PassNode* passNode = *it;

        VOX_ASSERT(passNode->isCulled() == false, "There must not be culled nodes after culling");

        DependencyGraph::EdgeContainer reads = _dependencyGraph.getIncomingEdges(passNode->getNodeID());
        for (const DependencyGraph::Edge* edge : reads)
        {
            DependencyGraph::Node* node = _dependencyGraph.getNode(edge->_fromNodeID);
            passNode->registerResource(this, static_cast<ResourceNode*>(node)->getResourceHandle());
        }

        DependencyGraph::EdgeContainer writes = _dependencyGraph.getOutgoingEdges(passNode->getNodeID());
        for (const DependencyGraph::Edge* edge : writes)
        {
            DependencyGraph::Node* node = _dependencyGraph.getNode(edge->_toNodeID);
            passNode->registerResource(this, static_cast<ResourceNode*>(node)->getResourceHandle());
        }

        passNode->resolve(this);
    }

    for (VirtualResource* resource : _resources)
    {
        if (resource->isCulled() == false)
        {
            PassNode* firstPass = resource->getFirstReferencedPassNode();
            PassNode* lastPass = resource->getLastReferencedPassNode();

            firstPass->addDevirtualize(resource);
            lastPass->addDestroy(resource);
        }
    }

    for (ResourceNode* resourceNode : _resourceNodes)
    {
        resourceNode->resolveResourceUsage(this);
    }

#else

#endif

    return true;
}

ResourceHandle FrameGraph::readInternal(ResourceHandle id, [[maybe_unused]] PassNode* passNode, std::function<bool(ResourceNode*, VirtualResource*)>&& connect)
{
    VOX_ASSERT(id < static_cast<ResourceHandle>(_resourceSlots.size()), "Invalid ResourceHandle({}) is given", id.get());

    const ResourceSlot& resourceSlot = getResourceSlot(id);
    ResourceNode* resourceNode = _resourceNodes[resourceSlot._nodeIndex];
    VirtualResource* vResource = _resources[resourceSlot._resourceIndex];

#if defined(VOXFLOW_DEBUG)
    DependencyGraph::EdgeContainer incomingEdges = _dependencyGraph.getIncomingEdges(resourceNode->getNodeID());
    for (const DependencyGraph::Edge* edge : incomingEdges)
    {
        if (edge->_fromNodeID == passNode->getNodeID())
        {
            VOX_ASSERT(false, "This resource is already written by same pass node");
        }
    }
#endif  // VOXFLOW_DEBUG

    if (connect(resourceNode, vResource))
    {
        // TODO(snowapril) : add post-jobs after connecting two nodes
    }

    // _dependencyGraph.link(resourceNode->getNodeID(), passNode->getNodeID());

    return id;
}

ResourceHandle FrameGraph::writeInternal(ResourceHandle id, PassNode* passNode, std::function<bool(ResourceNode*, VirtualResource*)>&& connect)
{
    VOX_ASSERT(id < static_cast<ResourceHandle>(_resourceSlots.size()), "Invalid ResourceHandle({}) is given", id.get());

    const ResourceSlot& resourceSlot = getResourceSlot(id);
    VirtualResource* vResource = _resources[resourceSlot._resourceIndex];
    ResourceNode* resourceNode = _resourceNodes[resourceSlot._nodeIndex];
    // const DependencyGraph::NodeID resourceNodeID = resourceNode->getNodeID();

    DependencyGraph::EdgeContainer outgoingEdges = _dependencyGraph.getOutgoingEdges(passNode->getNodeID());

    // bool alreadyWritten = false;
    // for (const DependencyGraph::Edge* edge : outgoingEdges)
    //{
    //     if (edge->_toNodeID == resourceNodeID)
    //     {
    //         alreadyWritten = true;
    //         break;
    //     }
    // }
    //
    // if (alreadyWritten)
    //{
    //     // TODO(snowapril) : update resource usage or something else
    // }
    // else
    //{
    //     _dependencyGraph.link(passNode->getNodeID(), resourceNode->getNodeID());
    // }

    if (connect(resourceNode, vResource))
    {
        // TODO(snowapril) : add post-jobs after connecting two nodes
    }

    // If the given pass is writing to imported resource, it must not be culled.
    if (vResource->isImported())
    {
        passNode->setSideEffectPass();
    }

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

            for (const DependencyGraph::Edge* writeEdge : _dependencyGraph.getOutgoingEdges(_passNodes[i]->getNodeID()))
            {
                for (const DependencyGraph::Edge* readEdge : _dependencyGraph.getIncomingEdges(_passNodes[j]->getNodeID()))
                {
                    if (writeEdge->_toNodeID == readEdge->_fromNodeID)
                    {
                        singleAdjacencyList.emplace_back(j);
                    }
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
                maxDistances = std::max(maxDistances, sDistances[connectedNodeIndex]);
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

    for (std::vector<PassNode*>::iterator iter = _passNodes.begin(); iter != _passNodeLast; ++iter)
    {
        PassNode* passNode = *iter;

        for (VirtualResource* resource : passNode->getDevirtualizes())
        {
            resource->devirtualize(_renderResourceAllocator);
        }

        FrameGraphResources resources(this, passNode);

        passNode->execute(&resources, _cmdStream);

        for (VirtualResource* resource : passNode->getDestroyes())
        {
            resource->destroy(_renderResourceAllocator);
        }
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

class AlphabetPermutator
{
 public:
    explicit AlphabetPermutator(uint32_t numNodes)
    {
        constexpr uint32_t numLowerAlphabets = 'z' - 'a' + 1;
        uint32_t numPermutationIndex = 0;
        while (numNodes > 0)
        {
            numNodes /= numLowerAlphabets;
            ++numPermutationIndex;
        }
        _permutationIndices.resize(numPermutationIndex, 0);
    }

    std::string getNextAlphabetPermutation()
    {
        std::string alphabetPermutation;
        bool needToIncrementAlphabet = true;
        for (uint8_t& permutationIndex : _permutationIndices)
        {
            alphabetPermutation += 'a' + permutationIndex;

            if (needToIncrementAlphabet)
            {
                if (permutationIndex == ('z' - 'a' + 1))
                {
                    permutationIndex = 0;
                    needToIncrementAlphabet = true;
                }
                else
                {
                    permutationIndex++;
                    needToIncrementAlphabet = false;
                }
            }
        }
        return alphabetPermutation;
    }

 private:
    std::vector<uint8_t> _permutationIndices;
};

void FrameGraph::dumpGraphViz(std::ostringstream& osstr)
{
    osstr << "digraph D {\n\tnode [fontname=\"Arial\"];\n\tedge "
             "[style=dashed];\n";

    std::map<DependencyGraph::NodeID, std::string> nodeLabelMap;
    const uint32_t numTotalNodes = static_cast<uint32_t>(_passNodes.size() + _resourceNodes.size());

    AlphabetPermutator permutator(numTotalNodes);

    for (PassNode* passNode : _passNodes)
    {
        const std::string& nodeLabel = permutator.getNextAlphabetPermutation();
        nodeLabelMap[passNode->getNodeID()] = nodeLabel;
        osstr << "\t" << nodeLabel << "[ label=\"" << passNode->getPassName() << "\" shape=record bgcolor=\"grey\" "
              << "style=\"filled" << (passNode->isCulled() ? ",dashed\"" : "\"") << "];\n";
    }
    osstr << '\n';

    for (ResourceNode* resourceNode : _resourceNodes)
    {
        VirtualResource* vresource = _resources[getResourceSlot(resourceNode->getResourceHandle())._resourceIndex];

        const std::string& nodeLabel = permutator.getNextAlphabetPermutation();
        nodeLabelMap[resourceNode->getNodeID()] = nodeLabel;
        osstr << "\t" << nodeLabel << "[ label=\"" << vresource->getResourceName() << "\" shape=label " << (resourceNode->isCulled() ? "style=dashed" : "")
              << "];\n";
    }
    osstr << '\n';

    for (const DependencyGraph::Edge* edge : _dependencyGraph.getLinkedEdges())
    {
        osstr << '\t' << nodeLabelMap[edge->_fromNodeID] << " -> " << nodeLabelMap[edge->_toNodeID] << '\n';
    }
    osstr << '}';
}
}  // namespace RenderGraph

}  // namespace VoxFlow