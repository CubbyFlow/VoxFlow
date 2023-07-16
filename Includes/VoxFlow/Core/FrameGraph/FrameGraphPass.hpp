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
class CommandJobSystem;

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
                         CommandJobSystem* commandJobSystem) = 0;
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
                 CommandJobSystem* commandJobSystem) final
    {
        std::invoke(_executionPhaseLambda, frameGraph, _resourceData,
                    commandJobSystem);
    }

 private:
    PassDataType _resourceData{};
    ExecutePhase _executionPhaseLambda;
};

class PassNode : public DependencyGraph::Node
{
 public:
    explicit PassNode(
        FrameGraph* ownerFrameGraph, std::string_view&& passName);
    ~PassNode() override;
    PassNode(PassNode&& passNode);
    PassNode& operator=(PassNode&& passNode);

    virtual void execute(FrameGraph* frameGraph,
                         CommandJobSystem* commandJobSystem) = 0;

    void setSideEffectPass()
    {
        _refCount = UINT32_MAX;
    }

    inline const std::string& getPassName() const
    {
        return _passName;
    }

 protected:
    std::vector<VirtualResource*> _devirtualizes;
    std::vector<VirtualResource*> _destroyes;
    std::string _passName;
    bool _hasSideEffect = false;
};

class RenderPassNode final : public PassNode
{
 public:
    explicit RenderPassNode(FrameGraph* ownerFrameGraph,
                            std::string_view&& passName,
                      std::unique_ptr<FrameGraphPassBase>&& pass);
    ~RenderPassNode() override;
    RenderPassNode(RenderPassNode&& passNode);
    RenderPassNode& operator=(RenderPassNode&& passNode);

    void execute(FrameGraph* frameGraph, CommandJobSystem* commandJobSystem) override
    {
        _passImpl->execute(frameGraph, commandJobSystem);
    }

 protected:
 private:
    std::unique_ptr<FrameGraphPassBase> _passImpl = nullptr;
};

class PresentPassNode final : public PassNode
{
 public:
    explicit PresentPassNode(FrameGraph* ownerFrameGraph,
                             std::string_view&& passName);
    ~PresentPassNode() final;
    PresentPassNode(PresentPassNode&& passNode);
    PresentPassNode& operator=(PresentPassNode&& passNode);

    void execute(FrameGraph* frameGraph, CommandJobSystem* commandJobSystem) override
    {
        (void)frameGraph;
        (void)commandJobSystem;
    }
};
}  // namespace FrameGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/FrameGraphPass-Impl.hpp>