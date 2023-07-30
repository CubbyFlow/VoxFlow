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
    // TODO(snowapril) : initialize tonemap pipeline and quad buffer
    _quadBuffer = std::make_unique<Buffer>(
        "QuadBuffer", _logicalDevice,
        _logicalDevice->getDeviceDefaultResourceMemoryPool());
    
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