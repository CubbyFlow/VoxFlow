// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
#include <VoxFlow/Core/Resources/ResourceUploadContext.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Editor/RenderPass/SceneObjectPass.hpp>

namespace VoxFlow
{

SceneObjectPass::SceneObjectPass(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
}

SceneObjectPass::~SceneObjectPass()
{
}

const std::vector<glm::vec3> cubeVertices = {
    { -0.5f, -0.5f, 0.5f }, { 0.5f, -0.5f, 0.5f },   { -0.5f, 0.5f, 0.5f },
    { 0.5f, 0.5f, 0.5f },   { -0.5f, -0.5f, -0.5f }, { 0.5f, -0.5f, -0.5f },
    { -0.5f, 0.5f, -0.5f }, { 0.5f, 0.5f, -0.5f },
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
    _sceneObjectPipeline = std::make_unique<GraphicsPipeline>(
        _logicalDevice, std::initializer_list<const char*>{
                            RESOURCES_DIR "/Shaders/scene_object.vert",
                            RESOURCES_DIR "/Shaders/scene_object.frag" });

    _cubeVertexBuffer = std::make_unique<Buffer>(
        "QuadVertexBuffer", _logicalDevice,
        _logicalDevice->getDeviceDefaultResourceMemoryPool());
    _cubeVertexBuffer->makeAllocationResident(BufferInfo{
        ._size = cubeVertices.size() * sizeof(glm::vec3),
        ._usage = BufferUsage::VertexBuffer | BufferUsage::CopyDst });

    _cubeIndexBuffer = std::make_unique<Buffer>(
        "QuadIndexBuffer", _logicalDevice,
        _logicalDevice->getDeviceDefaultResourceMemoryPool());
    _cubeIndexBuffer->makeAllocationResident(BufferInfo{
        ._size = cubeIndices.size() * sizeof(uint32_t),
        ._usage = BufferUsage::IndexBuffer | BufferUsage::CopyDst });

    return true;
}

void SceneObjectPass::updateRender(ResourceUploadContext* uploadContext)
{
    uploadContext->addPendingUpload(
        UploadPhase::Immediate, _cubeVertexBuffer.get(),
        UploadData{ ._data = &cubeVertices[0].x,
                    ._size = cubeVertices.size() * sizeof(glm::vec3),
                    ._dstOffset = 0 });

    uploadContext->addPendingUpload(
        UploadPhase::Immediate, _cubeIndexBuffer.get(),
        UploadData{ ._data = &cubeIndices[0],
                    ._size = cubeIndices.size() * sizeof(uint32_t),
                    ._dstOffset = 0 });
}

void SceneObjectPass::renderScene(RenderGraph::FrameGraph* frameGraph)
{
    using namespace RenderGraph;

    BlackBoard& blackBoard = frameGraph->getBlackBoard();
    ResourceHandle backBufferHandle =
        blackBoard.getHandle("BackBuffer");

    const auto& backBufferDesc =
        frameGraph->getResourceDescriptor<FrameGraphTexture>(
            backBufferHandle);

    _passData = frameGraph->addCallbackPass<SceneObjectPassData>(
        "SceneObjectPass",
        [&](FrameGraphBuilder& builder,
            SceneObjectPassData& passData) {
            passData._sceneColorHandle =
                builder.allocate<FrameGraphTexture>(
                    "SceneColor",
                    FrameGraphTexture::Descriptor{
                        ._width = backBufferDesc._width,
                        ._height = backBufferDesc._height,
                        ._depth = backBufferDesc._depth,
                        ._level = backBufferDesc._level,
                        ._sampleCounts = backBufferDesc._sampleCounts,
                        ._format = backBufferDesc._format });

            passData._sceneDepthHandle =
                builder.allocate<FrameGraphTexture>(
                    "SceneDepth", FrameGraphTexture::Descriptor{
                                      ._width = backBufferDesc._width,
                                      ._height = backBufferDesc._height,
                                      ._depth = 1,
                                      ._level = backBufferDesc._level,
                                      ._sampleCounts = 1,
                                      ._format = VK_FORMAT_D32_SFLOAT_S8_UINT });

            builder.write<FrameGraphTexture>(passData._sceneColorHandle, TextureUsage::RenderTarget);
            builder.write<FrameGraphTexture>(passData._sceneDepthHandle, TextureUsage::DepthStencil);

            auto descriptor = FrameGraphRenderPass::Descriptor{
                ._viewportSize =
                    glm::uvec2(backBufferDesc._width, backBufferDesc._height),
                ._writableAttachment = AttachmentMaskFlags::Color0,
                ._numSamples = 1
            };
            descriptor._attachments[0] = passData._sceneColorHandle;

            passData._renderPassID = builder.declareRenderPass(
                "SceneObjectPass RenderPass", std::move(descriptor));

            blackBoard["SceneColor"] = passData._sceneColorHandle;
            blackBoard["SceneDepth"] = passData._sceneDepthHandle;
        },
        [&](const FrameGraphResources* fgResources,
            SceneObjectPassData& passData, CommandStream* cmdStream) {
            RenderPassData* rpData =
                fgResources->getRenderPassData(passData._renderPassID);

            cmdStream->addJob(CommandJobType::BeginRenderPass,
                              rpData->_attachmentGroup, rpData->_passParams);
            cmdStream->addJob(CommandJobType::BindPipeline,
                              _sceneObjectPipeline.get());
            
            cmdStream->addJob(CommandJobType::BindVertexBuffer,
                              _cubeVertexBuffer);

            cmdStream->addJob(CommandJobType::BindIndexBuffer,
                              _cubeIndexBuffer);
            
            const auto& sceneColorDesc =
                fgResources
                    ->getResourceDescriptor<FrameGraphTexture>(
                        passData._sceneColorHandle);

            cmdStream->addJob(
                CommandJobType::SetViewport,
                glm::uvec2(sceneColorDesc._width, sceneColorDesc._height));

            cmdStream->addJob(CommandJobType::DrawIndexed, 36, 1, 0, 0, 0);

            cmdStream->addJob(CommandJobType::EndRenderPass);
        });
}

}  // namespace VoxFlow