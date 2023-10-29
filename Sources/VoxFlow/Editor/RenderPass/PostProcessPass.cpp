// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStreamingContext.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ResourceBindingLayout.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/ResourceUploadContext.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Editor/RenderPass/PostProcessPass.hpp>

namespace VoxFlow
{

PostProcessPass::PostProcessPass(LogicalDevice* logicalDevice) : _logicalDevice(logicalDevice)
{
}

PostProcessPass::~PostProcessPass()
{
}

bool PostProcessPass::initialize()
{
    _toneMapPipeline = _logicalDevice->getPipelineStreamingContext()->createGraphicsPipeline({ "tonemap.vert", "tonemap.frag" });

    // Set pipeline state for ToneMap PostProcess pipeline
    GraphicsPipelineState pipelineState;
    pipelineState.blendState.addBlendState().setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                                               VK_COLOR_COMPONENT_A_BIT);
    pipelineState.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
    _toneMapPipeline->setPipelineState(pipelineState);

    return true;
}

void PostProcessPass::updateRender(ResourceUploadContext* uploadContext)
{
    (void)uploadContext;
}

void PostProcessPass::renderScene(RenderGraph::FrameGraph* frameGraph)
{
    using namespace RenderGraph;

    BlackBoard& blackBoard = frameGraph->getBlackBoard();

    _passData = frameGraph->addCallbackPass<PostProcessPassData>(
        "PostProcessPass",
        [&](FrameGraphBuilder& builder, PostProcessPassData& passData) {
            ResourceHandle backBufferHandle = blackBoard.getHandle("BackBuffer");

            ResourceHandle sceneColorHandle = blackBoard.getHandle("SceneColor");

            const auto& sceneColorDesc = frameGraph->getResourceDescriptor<FrameGraphTexture>(sceneColorHandle);

            builder.read<FrameGraphTexture>(sceneColorHandle, TextureUsage::Sampled);
            builder.write<FrameGraphTexture>(backBufferHandle, TextureUsage::BackBuffer);

            auto descriptor = FrameGraphRenderPass::Descriptor{ ._viewportSize = glm::uvec2(sceneColorDesc._width, sceneColorDesc._height),
                                                                ._writableAttachment = AttachmentMaskFlags::Color0,
                                                                ._numSamples = 1 };
            descriptor._attachments[0] = backBufferHandle;

            passData._renderPassID = builder.declareRenderPass("PostProcess RenderPass", std::move(descriptor));
        },
        [&](const FrameGraphResources* fgResources, PostProcessPassData& passData, CommandStream* cmdStream) {
            RenderPassData* rpData = fgResources->getRenderPassData(passData._renderPassID);

            cmdStream->addJob(CommandJobType::BeginRenderPass, rpData->_attachmentGroup, rpData->_passParams);

            cmdStream->addJob(CommandJobType::BindPipeline, _toneMapPipeline.get());

            ResourceHandle sceneColorHandle = blackBoard.getHandle("SceneColor");
            TextureView* sceneColorView = fgResources->getTextureView(sceneColorHandle);

            cmdStream->addJob(CommandJobType::BindResourceGroup, SetSlotCategory::PerRenderPass,
                              std::vector<ShaderVariableBinding>{ ShaderVariableBinding{
                                  ._variableName = "g_sceneColor", ._view = sceneColorView, ._usage = ResourceAccessMask::ShaderReadOnly } });

            const auto& sceneColorDesc = fgResources->getResourceDescriptor<FrameGraphTexture>(sceneColorHandle);

            cmdStream->addJob(CommandJobType::SetViewport, glm::uvec2(sceneColorDesc._width, sceneColorDesc._height));

            cmdStream->addJob(CommandJobType::Draw, 4, 1, 0, 0);

            cmdStream->addJob(CommandJobType::EndRenderPass);
        });
}

}  // namespace VoxFlow