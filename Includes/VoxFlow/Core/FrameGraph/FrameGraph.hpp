// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_HPP
#define VOXEL_FLOW_FRAME_GRAPH_HPP

#include <VoxFlow/Core/FrameGraph/BlackBoard.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphRenderPass.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphResources.hpp>
#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/FrameGraph/ResourceHandle.hpp>
#include <VoxFlow/Core/FrameGraph/TypeTraits.hpp>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <functional>
#include <istream>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class CommandStream;
class RenderResourceAllocator;
class DependencyGraph;

namespace RenderGraph
{

class FrameGraph;

class FrameGraphBuilder
{
    friend class FrameGraph;

 public:
    FrameGraphBuilder() = delete;
    ~FrameGraphBuilder() = default;
    FrameGraphBuilder(const FrameGraphBuilder&) = delete;
    FrameGraphBuilder(FrameGraphBuilder&&) = delete;
    FrameGraphBuilder& operator=(const FrameGraphBuilder&) = delete;
    FrameGraphBuilder& operator=(FrameGraphBuilder&&) = delete;

 private:
    FrameGraphBuilder(FrameGraph* frameGraph, PassNode* passNode) : _frameGraph(frameGraph), _currentPassNode(passNode)
    {
    }

 public:
    template <ResourceConcept ResourceDataType>
    [[nodiscard]] ResourceHandle allocate(std::string&& resourceName, typename ResourceDataType::Descriptor&& initArgs);

    template <ResourceConcept ResourceDataType>
    ResourceHandle read(ResourceHandle id, typename ResourceDataType::Usage usage);

    template <ResourceConcept ResourceDataType>
    ResourceHandle write(ResourceHandle id, typename ResourceDataType::Usage usage);

    [[nodiscard]] uint32_t declareRenderPass(std::string_view&& passName, typename FrameGraphRenderPass::Descriptor&& initArgs);

    inline void setSideEffectPass()
    {
        _currentPassNode->setSideEffectPass();
    }

 protected:
 private:
    FrameGraph* _frameGraph = nullptr;
    PassNode* _currentPassNode = nullptr;
};

class FrameGraph : private NonCopyable
{
 public:
    explicit FrameGraph();
    ~FrameGraph();

 public:
    struct ResourceSlot
    {
        using IndexType = uint16_t;
        using VersionType = uint8_t;

        IndexType _resourceIndex = UINT16_MAX;
        IndexType _nodeIndex = UINT16_MAX;
        VersionType _version = UINT8_MAX;
    };

 public:
    template <typename PassDataType, typename SetupPhase, typename ExecutePhase>
    const PassDataType& addCallbackPass(std::string_view&& passName, SetupPhase&& setup, ExecutePhase&& execute);

    template <typename SetupPhase, typename ExecutePhase>
    void addCallbackPass(std::string_view&& passName, SetupPhase&& setup, ExecutePhase&& execute);

    template <typename SetupPhase>
    void addPresentPass(std::string_view&& passName, SetupPhase&& setup, SwapChain* swapChain, const FrameContext& frameContext);

    template <ResourceConcept ResourceDataType>
    [[nodiscard]] ResourceHandle create(std::string&& resourceName, typename ResourceDataType::Descriptor&& resourceDescArgs);

    [[nodiscard]] ResourceHandle importRenderTarget(std::string&& resourceName, FrameGraphTexture::Descriptor&& resourceDescArgs,
                                                    typename FrameGraphRenderPass::ImportedDescriptor&& importedDesc, TextureView* textureView);

    // Compile given frame graph
    bool compile();

    // Execute compiled frame graph
    void execute();

    // Clear per-frame frame graph metadata
    void clear();

    //
    void reset(CommandStream* cmdStream, RenderResourceAllocator* renderResourceAllocator);

    /**
     * Dump compiled graph as graphviz text into given string stream
     * @param isstr output string stream where compiled graph dumped at
     */
    void dumpGraphViz(std::ostringstream& osstr);

    inline void setLastSubmitFence(const FenceObject& fenceObject)
    {
        _lastSubmitFence = fenceObject;
    }
    inline FenceObject getLastSubmitFence() const
    {
        return _lastSubmitFence;
    }

 public:
    inline DependencyGraph* getDependencyGraph()
    {
        return &_dependencyGraph;
    }

    inline const BlackBoard& getBlackBoard() const
    {
        return _blackBoard;
    }

    inline BlackBoard& getBlackBoard()
    {
        return _blackBoard;
    }

    inline RenderResourceAllocator* getRenderResourceAllocator()
    {
        return _renderResourceAllocator;
    }

    VirtualResource* getVirtualResource(ResourceHandle resourceHandle)
    {
        const ResourceSlot& resourceSlot = getResourceSlot(resourceHandle);
        return _resources[resourceSlot._resourceIndex];
    }

    template <ResourceConcept ResourceDataType>
    const typename ResourceDataType::Descriptor getResourceDescriptor(ResourceHandle id) const;

 private:
    const ResourceSlot& getResourceSlot(ResourceHandle id) const
    {
        return _resourceSlots[id.get()];
    }

    ResourceSlot& getResourceSlot(ResourceHandle id)
    {
        return _resourceSlots[id.get()];
    }

    void buildAdjacencyLists(const uint32_t numPassNodes);
    bool topologicalSortPassNodes(const uint32_t numPassNodes);
    void calcDependencyLevels(const uint32_t numPassNodes);
    void allotCommandQueueIndices(const uint32_t numPassNodes);
    void buildSSIS(const uint32_t numPassNodes);
    void calcResourceLifetimes(const uint32_t numPassNodes);

 private:
    friend class FrameGraphBuilder;

    ResourceHandle readInternal(ResourceHandle id, [[maybe_unused]] PassNode* passNode, std::function<bool(ResourceNode*, VirtualResource*)>&& connect);
    ResourceHandle writeInternal(ResourceHandle id, PassNode* passNode, std::function<bool(ResourceNode*, VirtualResource*)>&& connect);

 private:
    std::vector<ResourceSlot> _resourceSlots;
    std::vector<PassNode*> _passNodes;
    std::vector<PassNode*>::iterator _passNodeLast;
    std::vector<ResourceNode*> _resourceNodes;
    std::vector<VirtualResource*> _resources;

    BlackBoard _blackBoard;
    std::vector<std::vector<uint32_t>> _passNodeAdjacencyList;
    std::vector<uint32_t> _topologicalSortedPassNodes;
    std::vector<std::vector<uint32_t>> _dependencyLevels;
    std::vector<uint32_t> _commandQueueIndices;

 private:
    DependencyGraph _dependencyGraph;
    CommandStream* _cmdStream = nullptr;
    RenderResourceAllocator* _renderResourceAllocator = nullptr;
    FenceObject _lastSubmitFence = FenceObject::Default();
};
}  // namespace RenderGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/FrameGraph-Impl.hpp>