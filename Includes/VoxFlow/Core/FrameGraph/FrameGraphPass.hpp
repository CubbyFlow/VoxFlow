// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_PASS_HPP
#define VOXEL_FLOW_FRAME_GRAPH_PASS_HPP

#include <VoxFlow/Core/FrameGraph/DependencyGraph.hpp>
#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace VoxFlow
{
class CommandExecutorBase;

namespace FrameGraph
{

class FrameGraph;
class PassNode;
class VirtualResource;

class FrameGraphPassBase : private NonCopyable
{
 public:
    explicit FrameGraphPassBase();
    ~FrameGraphPassBase();

    virtual void execute(FrameGraph* frameGraph,
                         CommandExecutorBase* commandExecutor) = 0;
};

template <typename PassDataType, typename ExecutePhase>
class FrameGraphPass : public FrameGraphPassBase
{
 public:
    explicit FrameGraphPass(ExecutePhase&& executePhase);
    ~FrameGraphPass();

    inline const PassDataType& getPassData() const
    {
        return _resourceData;
    }
    inline PassDataType& getPassData()
    {
        return _resourceData;
    }

    void execute(FrameGraph* frameGraph,
                 CommandExecutorBase* commandExecutor) final
    {
        std::invoke(_executionPhaseLambda, frameGraph, _resourceData,
                    commandExecutor);
    }

 private:
    PassDataType _resourceData{};
    ExecutePhase _executionPhaseLambda;
};

class PassNode : public DependencyGraph::Node
{
 public:
    explicit PassNode(
        FrameGraph* ownerFrameGraph, std::string_view&& passName,
        std::unique_ptr<FrameGraphPassBase>&& pass);
    ~PassNode() final;
    PassNode(PassNode&& passNode);
    PassNode& operator=(PassNode&& passNode);

    void execute(FrameGraph* frameGraph, CommandExecutorBase* commandExecutor)
    {
        _passImpl->execute(frameGraph, commandExecutor);
    }

    void setSideEffectPass()
    {
        _refCount = UINT32_MAX;
    }

    inline const std::string& getPassName() const
    {
        return _passName;
    }

 protected:
 private:
    FrameGraph* _ownerFrameGraph = nullptr;
    std::unique_ptr<FrameGraphPassBase> _passImpl = nullptr;
    std::vector<VirtualResource*> _devirtualizes;
    std::vector<VirtualResource*> _destroyes;
    std::string _passName;
    bool _hasSideEffect = false;
};
}  // namespace FrameGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/FrameGraphPass-Impl.hpp>