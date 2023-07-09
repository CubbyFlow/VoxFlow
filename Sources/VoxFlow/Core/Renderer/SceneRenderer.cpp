// Author : snowapril

#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/Devices/SwapChain.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandExecutor.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderer.hpp>
#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
SceneRenderer::SceneRenderer(LogicalDevice* logicalDevice,
                             FrameGraph::FrameGraph* frameGraph)
    : _logicalDevice(logicalDevice), _frameGraph(frameGraph)
{
}

SceneRenderer::~SceneRenderer()
{
}

bool SceneRenderer::initialize()
{
    // TODO(snowapril) : pass proper arguments to framegraph
    return true;
}

void SceneRenderer::beginFrameGraph(const FrameContext& frameContext)
{
    _currentFrameContext = frameContext;

    _frameGraph->reset(nullptr, nullptr);

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

void SceneRenderer::renderScene()
{
}
void SceneRenderer::endFrame()
{
}

}  // namespace VoxFlow