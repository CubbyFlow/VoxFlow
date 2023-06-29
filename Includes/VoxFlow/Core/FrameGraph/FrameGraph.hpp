// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_HPP
#define VOXEL_FLOW_FRAME_GRAPH_HPP

#include <VoxFlow/Core/FrameGraph/TypeTraits.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/FrameGraph/BlackBoard.hpp>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <istream>

namespace VoxFlow
{
class LogicalDevice;
class RenderResourceMemoryPool;
class CommandExecutorBase;
class RenderResourceAllocator;
class DependencyGraph;

namespace FrameGraph
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
    FrameGraphBuilder(FrameGraph* frameGraph, PassNode* passNode)
        : _frameGraph(frameGraph), _currentPassNode(passNode)
    {
    }

 public:
    template <ResourceConcept ResourceDataType>
    [[nodiscard]] ResourceHandle allocate(
        std::string_view&& resourceName,
        ResourceDataType::Descriptor&& initArgs);
    [[nodiscard]] ResourceHandle read(ResourceHandle id);
    [[nodiscard]] ResourceHandle write(ResourceHandle id);

    template <RenderPassConcept RenderPassType>
    [[nodiscard]] ResourceHandle declareRenderPass(
        std::string_view&& passName, RenderPassType::Descriptor&& initArgs);

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
    const PassDataType& addCallbackPass(std::string_view&& passName,
                                        SetupPhase&& setup,
                                        ExecutePhase&& execute);

    template <ResourceConcept ResourceDataType>
    [[nodiscard]] ResourceHandle create(
        std::string_view&& resourceName,
        ResourceDataType::Descriptor&& resourceDescArgs);

    [[nodiscard]] ResourceHandle importRenderTarget(
        std::string_view&& resourceName,
        FrameGraphTexture::Descriptor&& resourceDescArgs, Texture* texture);

    // Compile given frame graph
    bool compile();

    // Execute compiled frame graph
    void execute();

    // Clear per-frame frame graph metadata
    void clear();

    //
    void reset(CommandExecutorBase* commandExecutor,
               RenderResourceAllocator* renderResourceAllocator);

    /**
     * Dump compiled graph as graphviz text into given string stream
     * @param isstr output string stream where compiled graph dumped at
     */
    void dumpGraphViz(std::ostringstream& osstr);

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

private:
    const ResourceSlot& getResourceSlot(ResourceHandle id) const
    {
        return _resourceSlots[id];
    }

    ResourceSlot& getResourceSlot(ResourceHandle id)
    {
        return _resourceSlots[id];
    }

    void buildAdjacencyLists(const uint32_t numPassNodes);
    bool topologicalSortPassNodes(const uint32_t numPassNodes);
    void calcDependencyLevels(const uint32_t numPassNodes);
    void allotCommandQueueIndices(const uint32_t numPassNodes);
    void buildSSIS(const uint32_t numPassNodes);
    void calcResourceLifetimes(const uint32_t numPassNodes);

 private:
    friend class FrameGraphBuilder;

    ResourceHandle readInternal(ResourceHandle id, PassNode* passNode);
    ResourceHandle writeInternal(ResourceHandle id, PassNode* passNode);


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
    CommandExecutorBase* _commandExecutor = nullptr;
    RenderResourceAllocator* _renderResourceAllocator = nullptr;
};
}
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/FrameGraph-Impl.hpp>