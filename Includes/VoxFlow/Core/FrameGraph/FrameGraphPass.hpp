// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_PASS_HPP
#define VOXEL_FLOW_FRAME_GRAPH_PASS_HPP

#include <VoxFlow/Core/FrameGraph/FrameGraphRenderPass.hpp>
#include <VoxFlow/Core/FrameGraph/DependencyGraph.hpp>
#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPassParams.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderTargetGroup.hpp>
#include <functional>
#include <memory>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

namespace VoxFlow
{
class CommandStream;
class SwapChain;

namespace FrameGraph
{

class FrameGraph;
class FrameGraphBuilder;
class FrameGraphResources;
class PassNode;
class VirtualResource;

class FrameGraphPassBase : private NonCopyable
{
 public:
    explicit FrameGraphPassBase();
    ~FrameGraphPassBase();

    virtual void execute(const FrameGraphResources* resources,
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

    void execute(const FrameGraphResources* resources,
                 CommandStream* cmdStream) override final
    {
        std::invoke(_executionPhaseLambda, resources, _resourceData, cmdStream);
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

    virtual void execute(const FrameGraphResources* resources,
                         CommandStream* cmdStream) = 0;

    void setSideEffectPass()
    {
        _refCount = UINT32_MAX;
    }

    inline const std::string& getPassName() const
    {
        return _passName;
    }

    inline const std::unordered_set<ResourceHandle>& getDeclaredHandles() const
    {
        return _declaredHandles;
    }

    inline const std::vector<VirtualResource*>& getDevirtualizes() const
    {
        return _devirtualizes;
    }

    inline const std::vector<VirtualResource*>& getDestroyes() const
    {
        return _destroyes;
    }

    void registerResource(FrameGraph* frameGraph, ResourceHandle resourceHandle);
    void addDevirtualize(VirtualResource* resource);
    void addDestroy(VirtualResource* resource);
    void resolve(FrameGraph* frameGraph);

 protected:
    std::unordered_set<ResourceHandle> _declaredHandles;
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
        std::string_view _renderPassName;
        FrameGraphRenderPass::Descriptor _descriptor;
        AttachmentMaskFlags _blendMask = AttachmentMaskFlags::All;
        AttachmentGroup _attachmentGroup;
        RenderPassParams _passParams;
    };

 public:
    explicit RenderPassNode(FrameGraph* ownerFrameGraph,
                            std::string_view&& passName,
                      std::unique_ptr<FrameGraphPassBase>&& pass);
    ~RenderPassNode() override;
    RenderPassNode(RenderPassNode&& passNode);
    RenderPassNode& operator=(RenderPassNode&& passNode);

    void execute(const FrameGraphResources* resources,
                 CommandStream* cmdStream) override;

    /**
     * @param frameGraph owner frameGraph of this node
     * @param builder frame graph builder which is currently declaring this node
     * @param name render pass name
     * @param descriptor render pass descriptor to declare
     * @return resource handle of created render pass data
     */
    [[nodiscard]] ResourceHandle declareRenderPass(
        FrameGraph* frameGraph, FrameGraphBuilder* builder,
        std::string_view&& name,
        typename FrameGraphRenderPass::Descriptor&& descriptor);

    /**
     * @param render pass id of render pass data to return
     * @return render pass data corresponding to given render pass id
     */
    [[nodiscard]] inline RenderPassData const* getRenderPassData(ResourceHandle rpID) const noexcept
    {
        return &_renderPassData[rpID];
    }

 protected:
 private:
    std::unique_ptr<FrameGraphPassBase> _passImpl = nullptr;
    std::vector<RenderPassData> _renderPassData;
};

class PresentPassNode final : public PassNode
{
 public:
    explicit PresentPassNode(FrameGraph* ownerFrameGraph,
                             std::string_view&& passName,
                             SwapChain* swapChainToPresent,
                             const FrameContext& frameContext);
    ~PresentPassNode() final;
    PresentPassNode(PresentPassNode&& passNode);
    PresentPassNode& operator=(PresentPassNode&& passNode);

    void execute(const FrameGraphResources* resources,
                 CommandStream* cmdStream) override;

 private:
    SwapChain* _swapChainToPresent = nullptr;
    FrameContext _frameContext;
};
}  // namespace FrameGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/FrameGraphPass-Impl.hpp>