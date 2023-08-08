// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_PASS_HPP
#define VOXEL_FLOW_FRAME_GRAPH_PASS_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraphRenderPass.hpp>
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
class CommandStream;

namespace FrameGraph
{

class FrameGraph;
class FrameGraphBuilder;
class PassNode;
class VirtualResource;

class FrameGraphPassBase : private NonCopyable
{
 public:
    explicit FrameGraphPassBase();
    ~FrameGraphPassBase();

    virtual void execute(FrameGraph* frameGraph,
                         CommandStream* cmdStream) = 0;
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

    void execute(FrameGraph* frameGraph, CommandStream* cmdStream) final
    {
        std::invoke(_executionPhaseLambda, frameGraph, _resourceData,
                    cmdStream);
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
                         CommandStream* cmdStream) = 0;

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
    struct RenderPassData
    {
        FrameGraphRenderPass::Descriptor _descriptor;
    };

 public:
    explicit RenderPassNode(FrameGraph* ownerFrameGraph,
                            std::string_view&& passName,
                      std::unique_ptr<FrameGraphPassBase>&& pass);
    ~RenderPassNode() override;
    RenderPassNode(RenderPassNode&& passNode);
    RenderPassNode& operator=(RenderPassNode&& passNode);

    void execute(FrameGraph* frameGraph,
                 CommandStream* cmdStream) override
    {
        _passImpl->execute(frameGraph, cmdStream);
    }

    const RenderPassData* getRenderPassData(const uint32_t id) const
    {
        return &_renderPassData[id];
    }

    ResourceHandle declareRenderTarget(
        FrameGraph* frameGraph, FrameGraphBuilder* builder,
        std::string_view&& name,
        typename FrameGraphRenderPass::Descriptor&& descriptor);

 protected:
 private:
    std::unique_ptr<FrameGraphPassBase> _passImpl = nullptr;
    std::vector<RenderPassData> _renderPassData;
};

class PresentPassNode final : public PassNode
{
 public:
    explicit PresentPassNode(FrameGraph* ownerFrameGraph,
                             std::string_view&& passName);
    ~PresentPassNode() final;
    PresentPassNode(PresentPassNode&& passNode);
    PresentPassNode& operator=(PresentPassNode&& passNode);

    void execute(FrameGraph* frameGraph,
                 CommandStream* cmdStream) override
    {
        (void)frameGraph;
        (void)cmdStream;
    }
};
}  // namespace FrameGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/FrameGraphPass-Impl.hpp>