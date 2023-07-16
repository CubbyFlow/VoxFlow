// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderer.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>
#include <VoxFlow/Core/Utils/ChromeTracer.hpp>

namespace VoxFlow
{
SceneRenderer::SceneRenderer(LogicalDevice* logicalDevice,
                             FrameGraph::FrameGraph* frameGraph,
                             CommandJobSystem* commandJobSystem)
    : _logicalDevice(logicalDevice),
      _frameGraph(frameGraph),
      _commandJobSystem(commandJobSystem)
{
}

SceneRenderer::~SceneRenderer()
{
}

bool SceneRenderer::initialize()
{
    return true;
}

void SceneRenderer::beginFrameGraph(const FrameContext& frameContext)
{
    SCOPED_CHROME_TRACING("SceneRenderer::beginFrameGraph");

    _currentFrameContext = frameContext;

    _frameGraph->reset(_commandJobSystem, nullptr);

    SwapChain* swapChain =
        _logicalDevice->getSwapChain(_currentFrameContext._swapChainIndex)
            .get();
    Texture* currentBackBuffer =
        swapChain->getSwapChainImage(_currentFrameContext._backBufferIndex)
            .get();

    const TextureInfo& backBufferInfo = currentBackBuffer->getTextureInfo();

    auto backBufferDescriptor = FrameGraph::FrameGraphTexture::Descriptor{
        ._width = backBufferInfo._extent.x,
        ._height = backBufferInfo._extent.y,
        ._depth = backBufferInfo._extent.z,
        ._level = 0,
        ._sampleCounts = 1,
        ._format = backBufferInfo._format,
    };

    FrameGraph::BlackBoard& blackBoard = _frameGraph->getBlackBoard();

    FrameGraph::ResourceHandle backBufferHandle =
        _frameGraph->importRenderTarget(
            "BackBuffer", std::move(backBufferDescriptor), currentBackBuffer);
    blackBoard["BackBuffer"] = backBufferHandle;
}

tf::Future<void> SceneRenderer::resolveSceneRenderPasses()
{
    SCOPED_CHROME_TRACING("SceneRenderer::resolveSceneRenderPasses");

    tf::Taskflow taskflow;
    std::unordered_map<std::string, tf::Task> tasks;

    // Prepare tasks from registered scene render passes
    for (const auto& [passName, pass] : _sceneRenderPasses)
    {
        tf::Task fgTask =
            taskflow
                .emplace([this, &pass]() { pass->renderScene(_frameGraph); })
                .name(passName);

        tasks.emplace(passName, std::move(fgTask));
    }

    // Resolve dependency between tasks
    for (const auto& [passName, pass] : _sceneRenderPasses)
    {
        const std::vector<std::string>* dependentPasses =
            pass->getDepenentPasses();
        tf::Task& fgTask = tasks.find(passName)->second;

        for (const std::string& dependentPassName : *dependentPasses)
        {
            fgTask.succeed(tasks.find(dependentPassName)->second);
        }
    }

    // Add present pass which followed by all other passes
    FrameGraph::ResourceHandle backBufferHandle =
        _frameGraph->getBlackBoard().getHandle("BackBuffer");

    tf::Task presentTask =
        taskflow
            .emplace([&]() {
                _frameGraph->addPresentPass(
                    "PresentPass",
                    std::move([&](FrameGraph::FrameGraphBuilder& builder) {
                        builder.read(backBufferHandle);
                    }));
            })
            .name("PresentPass");

    for (const auto& [passName, pass] : _sceneRenderPasses)
    {
        tf::Task& fgTask = tasks.find(passName)->second;

        fgTask.precede(presentTask);
    }

    // Add frame graph compilation task
    taskflow.emplace([&]() { _frameGraph->compile(); })
        .name("Compilation")
        .succeed(presentTask);

    tf::Executor executor;
    return executor.run(taskflow);
}

void SceneRenderer::submitFrameGraph()
{
    SCOPED_CHROME_TRACING("SceneRenderer::submitFrameGraph");
    _frameGraph->execute();
}

}  // namespace VoxFlow