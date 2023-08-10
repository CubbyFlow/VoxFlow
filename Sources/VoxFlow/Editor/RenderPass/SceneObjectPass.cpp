// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
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

bool SceneObjectPass::initialize(ResourceUploadContext* uploadContext)
{
    _sceneObjectPipeline = std::make_unique<GraphicsPipeline>(
        _logicalDevice, std::initializer_list<const char*>{
                            RESOURCES_DIR "/Shaders/test_shader.vert",
                            RESOURCES_DIR "/Shaders/test_shader.vert" });

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
                            RESOURCES_DIR "/Shaders/test_shader.vert",
                            RESOURCES_DIR "/Shaders/test_shader.vert" });

    _cubeVertexBuffer = std::make_unique<Buffer>(
        "QuadVertexBuffer", _logicalDevice,
        _logicalDevice->getDeviceDefaultResourceMemoryPool());
    _cubeVertexBuffer->makeAllocationResident(BufferInfo{
        ._size = sizeof(cubeVertices),
        ._usage = BufferUsage::VertexBuffer | BufferUsage::CopyDst });

    uploadContext->addPendingUpload(UploadPhase::Immediate,
                                    _cubeVertexBuffer.get(),
                                    UploadData{ ._data = &cubeVertices[0].x,
                                                ._size = sizeof(cubeVertices),
                                                ._dstOffset = 0 });

    _cubeIndexBuffer = std::make_unique<Buffer>(
        "QuadIndexBuffer", _logicalDevice,
        _logicalDevice->getDeviceDefaultResourceMemoryPool());
    _cubeIndexBuffer->makeAllocationResident(BufferInfo{
        ._size = sizeof(cubeIndices),
        ._usage = BufferUsage::IndexBuffer | BufferUsage::CopyDst });

    return true;
}

void SceneObjectPass::updateRender(ResourceUploadContext* uploadContext)
{
    uploadContext->addPendingUpload(UploadPhase::Immediate,
                                    _cubeVertexBuffer.get(),
                                    UploadData{ ._data = &cubeVertices[0].x,
                                                ._size = sizeof(cubeVertices),
                                                ._dstOffset = 0 });

    uploadContext->addPendingUpload(UploadPhase::Immediate,
                                    _cubeIndexBuffer.get(),
                                    UploadData{ ._data = &cubeIndices[0],
                                                ._size = sizeof(cubeIndices),
                                                ._dstOffset = 0 });

    return true;
}

void SceneObjectPass::renderScene(FrameGraph::FrameGraph* frameGraph)
{
    FrameGraph::ResourceHandle backBufferHandle =
        frameGraph->getBlackBoard().getHandle("BackBuffer");

    struct TempPassData
    {
    };

    frameGraph->addCallbackPass<TempPassData>(
        "SceneObjectPass",
        [&](FrameGraph::FrameGraphBuilder& builder, TempPassData&) {
            builder.write(backBufferHandle);
        },
        [&](FrameGraph::FrameGraph*, TempPassData&, CommandStream*) {});
}

}  // namespace VoxFlow