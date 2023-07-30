// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ResourceBindingLayout.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Editor/RenderPass/PostProcessPass.hpp>

namespace VoxFlow
{

PostProcessPass::PostProcessPass(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
}

PostProcessPass::~PostProcessPass()
{
}

bool PostProcessPass::initialize()
{
    const glm::vec2 quadVertices[] = { glm::vec2(-1.0f, 1.0f),
                                       glm::vec2(1.0f, 1.0f),
                                       glm::vec2(-1.0f, -1.0f),
                                       glm::vec2(1.0f, -1.0f) };

    const uint32_t quadIndices[] = { 0, 1, 2, 1, 3, 2 };

    _quadVertexBuffer = std::make_unique<Buffer>(
        "QuadVertexBuffer", _logicalDevice,
        _logicalDevice->getDeviceDefaultResourceMemoryPool());
    _quadVertexBuffer->makeAllocationResident(BufferInfo{
        ._size = sizeof(quadVertices),
        ._usage = BufferUsage::VertexBuffer | BufferUsage::CopyDst });
    _quadVertexBuffer->upload(&quadVertices[0].x, 0, sizeof(quadVertices));

    _quadIndexBuffer = std::make_unique<Buffer>(
        "QuadIndexBuffer", _logicalDevice,
        _logicalDevice->getDeviceDefaultResourceMemoryPool());
    _quadIndexBuffer->makeAllocationResident(BufferInfo{
        ._size = sizeof(quadIndices),
        ._usage = BufferUsage::IndexBuffer | BufferUsage::CopyDst });
    _quadIndexBuffer->upload(&quadIndices[0], 0, sizeof(quadIndices));
    
    return true;
}

void PostProcessPass::renderScene(FrameGraph::FrameGraph* frameGraph)
{
    FrameGraph::ResourceHandle backBufferHandle =
        frameGraph->getBlackBoard().getHandle("BackBuffer");

    const auto& backBufferDesc =
        frameGraph->getResourceDescriptor<FrameGraph::FrameGraphTexture>(
            backBufferHandle);

    frameGraph->addCallbackPass<ToneMappingPassData>(
        "PostProcessPass",
        [&](FrameGraph::FrameGraphBuilder& builder,
            ToneMappingPassData& passData) {
            builder.read(backBufferHandle);
            passData._afterToneMap =
                builder.allocate<FrameGraph::FrameGraphTexture>(
                    "AfterToneMap",
                    FrameGraph::FrameGraphTexture::Descriptor{
                        ._width = backBufferDesc._width,
                        ._height = backBufferDesc._height,
                        ._depth = backBufferDesc._depth,
                        ._level = backBufferDesc._level,
                        ._sampleCounts = backBufferDesc._sampleCounts,
                        ._format = backBufferDesc._format });
            builder.write(passData._afterToneMap);
        },
        [&](FrameGraph::FrameGraph*, ToneMappingPassData&,
            CommandJobSystem* cmdJobSystem) {
            CommandBuffer* commandBuffer = cmdJobSystem->getCommandBuffer();
            commandBuffer->bindPipeline(_toneMapPipeline);
            commandBuffer->bindResourceGroup(
                SetSlotCategory::PerRenderPass,
                { ShaderVariable{ ._variableName = "ToneMapWeight",
                                  ._view = nullptr,
                                  ._usage = ResourceLayout::UniformBuffer } });
            commandBuffer->drawIndexed(4, 1, 0, 0, 0);
            commandBuffer->unbindPipeline();
        });
}

}  // namespace VoxFlow