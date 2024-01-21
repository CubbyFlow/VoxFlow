// Author : snowapril

#include <VoxFlow/Core/Devices/RenderDevice.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/PipelineStreamingContext.hpp>
#include <VoxFlow/Core/Scene/SceneObjectLoader.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/ResourceUploadContext.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Editor/RenderPass/SceneObjectPass.hpp>

namespace VoxFlow
{

SceneObjectPass::SceneObjectPass(RenderDevice* renderDevice) : _renderDevice(renderDevice)
{
}

SceneObjectPass::~SceneObjectPass()
{
}

const std::vector<glm::vec3> cubeVertices = {
    { -0.5f, -0.5f, 0.5f },  { 0.5f, -0.5f, 0.5f },  { -0.5f, 0.5f, 0.5f },  { 0.5f, 0.5f, 0.5f },
    { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f }, { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f },
};

const std::vector<unsigned int> cubeIndices = {
    0, 3, 2, 0, 1, 3,  //    6-----7     Y
    1, 7, 3, 1, 5, 7,  //   /|    /|     ^
    5, 4, 7, 6, 7, 4,  //  2-----3 |     |
    0, 6, 4, 0, 2, 6,  //  | 4 --|-5     ---> X
    2, 7, 6, 2, 3, 7,  //  |/    |/     /
    4, 1, 0, 4, 5, 1,  //  0-----1     Z
};

bool SceneObjectPass::initialize()
{
    LogicalDevice* logicalDevice = _renderDevice->getLogicalDevice(LogicalDeviceType::MainDevice);

    _sceneObjectPipeline = logicalDevice->getPipelineStreamingContext()->createGraphicsPipeline({ "scene_object.vert", "scene_object.frag" });

    // Set pipeline state for SceneObjectPass pipeline
    GraphicsPipelineState pipelineState;
    pipelineState.blendState.addBlendState().setColorWriteMask(VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT |
                                                               VK_COLOR_COMPONENT_A_BIT);
    pipelineState.inputLayout.addInputLayout(
        VertexInputLayout{ ._location = 0, ._binding = 0, ._stride = sizeof(glm::vec3), ._baseType = VertexFormatBaseType::Float32 });
    pipelineState.depthStencil.setDepth(true, VK_COMPARE_OP_LESS_OR_EQUAL);
    _sceneObjectPipeline->setPipelineState(pipelineState);

    _cubeVertexBuffer = std::make_unique<Buffer>("CubeVertexBuffer", logicalDevice, logicalDevice->getDeviceDefaultResourceMemoryPool());
    _cubeVertexBuffer->makeAllocationResident(
        BufferInfo{ ._size = cubeVertices.size() * sizeof(glm::vec3), ._usage = BufferUsage::VertexBuffer | BufferUsage::CopyDst });

    _cubeIndexBuffer = std::make_unique<Buffer>("CubeIndexBuffer", logicalDevice, logicalDevice->getDeviceDefaultResourceMemoryPool());
    _cubeIndexBuffer->makeAllocationResident(
        BufferInfo{ ._size = cubeIndices.size() * sizeof(uint32_t), ._usage = BufferUsage::IndexBuffer | BufferUsage::CopyDst });

    _sceneObjectCollection = std::make_unique<SceneObjectCollection>();

    SceneObjectLoader loader;
    loader.loadSceneObject(RESOURCES_DIR "/Scenes/Sponza/Sponza.gltf", _renderDevice->getResourceUploadContext(),
                           logicalDevice->getCommandJobSystem()->getCommandStream(
                               CommandStreamKey{ ._cmdStreamName = ASYNC_UPLOAD_STREAM_NAME, ._cmdStreamUsage = CommandStreamUsage::Transfer }),
                           _sceneObjectCollection.get());

    return true;
}

void SceneObjectPass::updateRender(ResourceUploadContext* uploadContext)
{
    uploadContext->addPendingUpload(UploadPhase::Immediate, _cubeVertexBuffer.get(),
                                    UploadData{ ._data = &cubeVertices[0].x, ._size = cubeVertices.size() * sizeof(glm::vec3), ._dstOffset = 0 });

    uploadContext->addPendingUpload(UploadPhase::Immediate, _cubeIndexBuffer.get(),
                                    UploadData{ ._data = &cubeIndices[0], ._size = cubeIndices.size() * sizeof(uint32_t), ._dstOffset = 0 });
}

void SceneObjectPass::renderScene(RenderGraph::FrameGraph* frameGraph)
{
    using namespace RenderGraph;

    BlackBoard& blackBoard = frameGraph->getBlackBoard();
    ResourceHandle backBufferHandle = blackBoard.getHandle("BackBuffer");

    const auto& backBufferDesc = frameGraph->getResourceDescriptor<FrameGraphTexture>(backBufferHandle);

    _passData = frameGraph->addCallbackPass<SceneObjectPassData>(
        "SceneObjectPass",
        [&](FrameGraphBuilder& builder, SceneObjectPassData& passData) {
            passData._sceneColorHandle =
                builder.allocate<FrameGraphTexture>("SceneColor", FrameGraphTexture::Descriptor{ ._width = backBufferDesc._width,
                                                                                                 ._height = backBufferDesc._height,
                                                                                                 ._depth = backBufferDesc._depth,
                                                                                                 ._level = backBufferDesc._level,
                                                                                                 ._sampleCounts = backBufferDesc._sampleCounts,
                                                                                                 ._format = backBufferDesc._format });

            passData._sceneDepthHandle =
                builder.allocate<FrameGraphTexture>("SceneDepth", FrameGraphTexture::Descriptor{ ._width = backBufferDesc._width,
                                                                                                 ._height = backBufferDesc._height,
                                                                                                 ._depth = 1,
                                                                                                 ._level = backBufferDesc._level,
                                                                                                 ._sampleCounts = 1,
                                                                                                 ._format = VK_FORMAT_D32_SFLOAT_S8_UINT });

            builder.write<FrameGraphTexture>(passData._sceneColorHandle, TextureUsage::RenderTarget);
            builder.write<FrameGraphTexture>(passData._sceneDepthHandle, TextureUsage::DepthStencil);

            auto descriptor = FrameGraphRenderPass::Descriptor{ ._viewportSize = glm::uvec2(backBufferDesc._width, backBufferDesc._height),
                                                                ._writableAttachment = AttachmentMaskFlags::Color0 | AttachmentMaskFlags::DepthStencil,
                                                                ._numSamples = 1 };
            descriptor._attachments[0] = passData._sceneColorHandle;
            descriptor._attachments[MAX_RENDER_TARGET_COUNTS] = passData._sceneDepthHandle;
            descriptor._clearFlags = AttachmentMaskFlags::Color0 | AttachmentMaskFlags::DepthStencil;
            descriptor._clearColors[0] = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            descriptor._clearDepth = 1.0f;

            passData._renderPassID = builder.declareRenderPass("SceneObjectPass RenderPass", std::move(descriptor));

            blackBoard["SceneColor"] = passData._sceneColorHandle;
            blackBoard["SceneDepth"] = passData._sceneDepthHandle;
        },
        [&](const FrameGraphResources* fgResources, SceneObjectPassData& passData, CommandStream* cmdStream) {
            RenderPassData* rpData = fgResources->getRenderPassData(passData._renderPassID);

            cmdStream->addJob(CommandJobType::BeginRenderPass, rpData->_attachmentGroup, rpData->_passParams);
            cmdStream->addJob(CommandJobType::BindPipeline, _sceneObjectPipeline.get());

            cmdStream->addJob(CommandJobType::BindVertexBuffer, _cubeVertexBuffer);

            cmdStream->addJob(CommandJobType::BindIndexBuffer, _cubeIndexBuffer);

            const auto& sceneColorDesc = fgResources->getResourceDescriptor<FrameGraphTexture>(passData._sceneColorHandle);

            cmdStream->addJob(CommandJobType::SetViewport, glm::uvec2(sceneColorDesc._width, sceneColorDesc._height));

            cmdStream->addJob(CommandJobType::DrawIndexed, 36, 1, 0, 0, 0);

            cmdStream->addJob(CommandJobType::EndRenderPass);
        });
}

}  // namespace VoxFlow