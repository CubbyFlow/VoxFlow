// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ResourceBindingLayout.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Resources/ResourceUploadContext.hpp>
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
    _toneMapPipeline = std::make_unique<GraphicsPipeline>(
        _logicalDevice, std::initializer_list<const char*>{
                            RESOURCES_DIR "/Shaders/tonemap.vert",
                            RESOURCES_DIR "/Shaders/tonemap.frag" });
    return true;
}

void PostProcessPass::updateRender(ResourceUploadContext* uploadContext)
{
    (void)uploadContext;
}

void PostProcessPass::renderScene(RenderGraph::FrameGraph* frameGraph)
{
    RenderGraph::BlackBoard& blackBoard = frameGraph->getBlackBoard();

    _passData = frameGraph->addCallbackPass<PostProcessPassData>(
        "PostProcessPass",
        [&](RenderGraph::FrameGraphBuilder& builder,
            PostProcessPassData& passData) {
            RenderGraph::ResourceHandle backBufferHandle =
                blackBoard.getHandle("BackBuffer");

            RenderGraph::ResourceHandle sceneColorHandle =
                blackBoard.getHandle("SceneColor");

            const auto& sceneColorDesc =
                frameGraph
                    ->getResourceDescriptor<RenderGraph::FrameGraphTexture>(
                        sceneColorHandle);

            builder.read(sceneColorHandle);
            builder.write(backBufferHandle);

            auto descriptor = RenderGraph::FrameGraphRenderPass::Descriptor{
                ._viewportSize =
                    glm::uvec2(sceneColorDesc._width, sceneColorDesc._height),
                ._writableAttachment = AttachmentMaskFlags::Color0,
                ._numSamples = 1
            };
            descriptor._attachments[0] = backBufferHandle;

            passData._renderPassID = builder.declareRenderPass(
                "PostProcess RenderPass", std::move(descriptor));
        },
        [&](const RenderGraph::FrameGraphResources* fgResources,
            PostProcessPassData& passData, CommandStream* cmdStream) {
            RenderGraph::RenderPassData* rpData =
                fgResources->getRenderPassData(passData._renderPassID);

            cmdStream->addJob(CommandJobType::BeginRenderPass,
                              rpData->_attachmentGroup, rpData->_passParams);

            cmdStream->addJob(CommandJobType::BindPipeline,
                              _toneMapPipeline.get());

            RenderGraph::ResourceHandle sceneColorHandle =
                blackBoard.getHandle("SceneColor");
            TextureView* sceneColorView =
                fgResources->getTextureView(sceneColorHandle);

            cmdStream->addJob(
                CommandJobType::BindResourceGroup,
                SetSlotCategory::PerRenderPass,
                std::vector<ShaderVariableBinding>{ ShaderVariableBinding{
                    ._variableName = "g_sceneColor",
                    ._view = sceneColorView,
                    ._usage = ResourceAccessMask::ShaderReadOnly } });

            const auto& sceneColorDesc =
                fgResources
                    ->getResourceDescriptor<RenderGraph::FrameGraphTexture>(
                        sceneColorHandle);

            cmdStream->addJob(
                CommandJobType::SetViewport,
                glm::uvec2(sceneColorDesc._width, sceneColorDesc._height));

            cmdStream->addJob(CommandJobType::Draw, 4, 1, 0, 0);

            cmdStream->addJob(CommandJobType::EndRenderPass);
        });
}

}  // namespace VoxFlow