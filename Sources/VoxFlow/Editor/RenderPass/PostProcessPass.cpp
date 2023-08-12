// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/ResourceBindingLayout.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
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

void PostProcessPass::renderScene(FrameGraph::FrameGraph* frameGraph)
{
    FrameGraph::ResourceHandle backBufferHandle =
        frameGraph->getBlackBoard().getHandle("BackBuffer");

    FrameGraph::ResourceHandle sceneColorHandle =
        frameGraph->getBlackBoard().getHandle("SceneColor");

    const auto& sceneColorDesc =
        frameGraph->getResourceDescriptor<FrameGraph::FrameGraphTexture>(
            sceneColorHandle);

    frameGraph->addCallbackPass(
        "PostProcessPass",
        [&](FrameGraph::FrameGraphBuilder& builder, auto&) {
            builder.read(sceneColorHandle);
            builder.write(backBufferHandle);
        },
        [&](const FrameGraph::FrameGraphResources*, auto&,
            CommandStream* cmdStream) {
            // cmdStream
            //     ->addJob(CommandJobType::BeginRenderPass, )

            cmdStream->addJob(CommandJobType::BindPipeline,
                              _toneMapPipeline.get());

            cmdStream->addJob(CommandJobType::BindResourceGroup,
                              SetSlotCategory::PerRenderPass,
                              std::vector<ShaderVariable>{ ShaderVariable{
                                  ._variableName = "ToneMapWeight",
                                  ._view = nullptr,
                                  ._usage = ResourceLayout::UniformBuffer } });

            cmdStream->addJob(CommandJobType::Draw, 4, 1, 0, 0);

            cmdStream->addJob(CommandJobType::EndRenderPass);
        });
}

}  // namespace VoxFlow