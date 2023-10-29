// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphPass.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphResources.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <glm/common.hpp>

namespace VoxFlow
{

namespace RenderGraph
{

FrameGraphPassBase::FrameGraphPassBase()
{
}

FrameGraphPassBase::~FrameGraphPassBase()
{
}

PassNode::PassNode(FrameGraph* ownerFrameGraph, std::string_view&& passName) : DependencyGraph::Node(ownerFrameGraph->getDependencyGraph()), _passName(passName)
{
}

PassNode ::~PassNode()
{
}

PassNode::PassNode(PassNode&& passNode) : DependencyGraph::Node(std::move(passNode))
{
    operator=(std::move(passNode));
}

PassNode& PassNode::operator=(PassNode&& passNode)
{
    if (this != &passNode)
    {
        _passName = std::move(passNode._passName);
        _hasSideEffect = passNode._hasSideEffect;
    }

    DependencyGraph::Node::operator=(std::move(passNode));
    return *this;
}

void PassNode::registerResource(FrameGraph* frameGraph, ResourceHandle resourceHandle)
{
    VirtualResource* resource = frameGraph->getVirtualResource(resourceHandle);
    resource->isReferencedByPass(this);
    _declaredHandles.insert(resourceHandle);
}

void PassNode::addDevirtualize(VirtualResource* resource)
{
    _devirtualizes.push_back(resource);
}

void PassNode::addDestroy(VirtualResource* resource)
{
    _destroyes.push_back(resource);
}

void RenderPassData::devirtualize(FrameGraph* frameGraph, RenderResourceAllocator* allocator)
{
    (void)allocator;

    std::vector<Attachment> colorAttachments;
    Attachment depthStencilAttachment(nullptr);

    for (uint32_t i = 0; i < MAX_RENDER_TARGET_COUNTS; ++i)
    {
        ResourceHandle colorHandle = _descriptor._attachments[i];
        if (colorHandle)
        {
            Resource<FrameGraphTexture>* resource = static_cast<Resource<FrameGraphTexture>*>(frameGraph->getVirtualResource(colorHandle));

            TextureView* attachmentView = nullptr;
            if (resource->isImported())
            {
                attachmentView = static_cast<ImportedRenderTarget*>(resource)->getTextureView();
            }
            else
            {
                attachmentView = resource->getInternalResource()._textureView;
            }

            colorAttachments.emplace_back(attachmentView);
        }
    }

    if (_descriptor._attachments[MAX_RENDER_TARGET_COUNTS])
    {
        Resource<FrameGraphTexture>* resource =
            static_cast<Resource<FrameGraphTexture>*>(frameGraph->getVirtualResource(_descriptor._attachments[MAX_RENDER_TARGET_COUNTS]));

        TextureView* attachmentView = nullptr;
        if (resource->isImported())
        {
            attachmentView = static_cast<ImportedRenderTarget*>(resource)->getTextureView();
        }
        else
        {
            attachmentView = resource->getInternalResource()._textureView;
        }

        depthStencilAttachment = Attachment(attachmentView);
    }

    _attachmentGroup = AttachmentGroup(std::move(colorAttachments), std::move(depthStencilAttachment), _descriptor._numSamples);
}

void RenderPassData::destroy(RenderResourceAllocator* allocator)
{
    (void)allocator;
}

RenderPassNode::RenderPassNode(FrameGraph* ownerFrameGraph, std::string_view&& passName, std::unique_ptr<FrameGraphPassBase>&& pass)
    : PassNode(ownerFrameGraph, std::move(passName)), _passImpl(std::move(pass))
{
}

RenderPassNode::~RenderPassNode()
{
    _passImpl.reset();
}

RenderPassNode::RenderPassNode(RenderPassNode&& passNode) : PassNode(std::move(passNode))
{
    operator=(std::move(passNode));
}

RenderPassNode& RenderPassNode::operator=(RenderPassNode&& passNode)
{
    if (this != &passNode)
    {
        _passImpl.swap(passNode._passImpl);
        _renderPassDatas.swap(passNode._renderPassDatas);
    }

    PassNode::operator=(std::move(passNode));
    return *this;
}

void RenderPassNode::execute(const FrameGraphResources* resources, CommandStream* cmdStream)
{
    FrameGraph* frameGraph = resources->getFrameGraph();
    RenderResourceAllocator* allocator = frameGraph->getRenderResourceAllocator();

    for (RenderPassData& passData : _renderPassDatas)
    {
        passData.devirtualize(frameGraph, allocator);
    }

    _passImpl->execute(resources, cmdStream);

    for (RenderPassData& passData : _renderPassDatas)
    {
        passData.destroy(allocator);
    }
}

uint32_t RenderPassNode::declareRenderPass(FrameGraph* frameGraph, FrameGraphBuilder* builder, std::string_view&& name,
                                           typename FrameGraphRenderPass::Descriptor&& descriptor)
{
    // TODO(snowapril) : implement
    (void)frameGraph;
    (void)builder;

    const uint32_t rpID = static_cast<uint32_t>(_renderPassDatas.size());

    RenderPassData renderPassData = { ._renderPassName = std::move(name), ._descriptor = std::move(descriptor) };
    _renderPassDatas.emplace_back(renderPassData);

    return rpID;
}

void RenderPassNode::resolve(FrameGraph* frameGraph)
{
    for (RenderPassData& rpData : _renderPassDatas)
    {
        uint32_t maxWidth = 0;
        uint32_t maxHeight = 0;

        for (ResourceHandle attachmentHandle : rpData._descriptor._attachments)
        {
            if (attachmentHandle)
            {
                Resource<FrameGraphTexture>* resource = static_cast<Resource<FrameGraphTexture>*>(frameGraph->getVirtualResource(attachmentHandle));

                const FrameGraphTexture::Descriptor descriptor = resource->getDescriptor();

                maxWidth = glm::max(descriptor._width, maxWidth);
                maxHeight = glm::max(descriptor._height, maxHeight);
            }
        }

        if ((rpData._descriptor._viewportSize.x != 0U) && (rpData._descriptor._viewportSize.y != 0U))
        {
            maxWidth = rpData._descriptor._viewportSize.x;
            maxHeight = rpData._descriptor._viewportSize.y;
        }

        rpData._passParams._attachmentFlags._clearFlags = rpData._descriptor._clearFlags;
        rpData._passParams._viewportSize = glm::uvec2(maxWidth, maxHeight);
        rpData._passParams._clearColors = rpData._descriptor._clearColors;
        rpData._passParams._clearDepth = rpData._descriptor._clearDepth;
        rpData._passParams._clearStencil = rpData._descriptor._clearStencil;
        rpData._passParams._writableAttachment = rpData._descriptor._writableAttachment;
    }
}

PresentPassNode::PresentPassNode(FrameGraph* ownerFrameGraph, std::string_view&& passName, SwapChain* swapChainToPresent, const FrameContext& frameContext)
    : PassNode(ownerFrameGraph, std::move(passName)), _swapChainToPresent(swapChainToPresent), _frameContext(frameContext)
{
}

PresentPassNode ::~PresentPassNode()
{
}

PresentPassNode::PresentPassNode(PresentPassNode&& passNode) : PassNode(std::move(passNode))
{
    operator=(std::move(passNode));
}

PresentPassNode& PresentPassNode::operator=(PresentPassNode&& passNode)
{
    PassNode::operator=(std::move(passNode));
    return *this;
}

void PresentPassNode::execute(const FrameGraphResources* resources, CommandStream* cmdStream)
{
    cmdStream->addJob(CommandJobType::MakeSwapChainFinalLayout, _swapChainToPresent, _frameContext._backBufferIndex);

    FenceObject executedFence = cmdStream->flush(_swapChainToPresent, &_frameContext, false);

    resources->getFrameGraph()->setLastSubmitFence(executedFence);
}

}  // namespace RenderGraph

}  // namespace VoxFlow