// Author : snowapril

#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp>
#include <VoxFlow/Core/Resources/Buffer.hpp>
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

bool SceneObjectPass::initialize()
{
    _sceneObjectPipeline = std::make_unique<GraphicsPipeline>(
        _logicalDevice, std::initializer_list<const char*>{
                            RESOURCES_DIR "/Shaders/test_shader.vert",
                            RESOURCES_DIR "/Shaders/test_shader.vert" });

    // TODO(snowapril) : create buffer with host_visible at now, later will be
    // replaced by staging buffer.
    _cubeVertexBuffer = std::make_unique<Buffer>(
        "CubeVertexBuffer", _logicalDevice,
        _logicalDevice->getDeviceDefaultResourceMemoryPool());

    BufferInfo bufferInfo = { ._size = 0, ._usage = BufferUsage::VertexBuffer };

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
        [&](FrameGraph::FrameGraph*, TempPassData&, CommandJobSystem*) {
            spdlog::info("SceneObjectPass");
        });
}

}  // namespace VoxFlow