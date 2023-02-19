// Author : snowapril

#include <VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/FrameBuffer.hpp>

namespace VoxFlow
{

RenderPassCollector::RenderPassCollector(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{

}

RenderPassCollector::~RenderPassCollector()
{
    release();
}

RenderPassCollector::RenderPassCollector(RenderPassCollector&& other) noexcept
{
    operator=(std::move(other));
}

RenderPassCollector& RenderPassCollector::operator=(
    RenderPassCollector&& other) noexcept
{
    if (&other != this)
    {
        _logicalDevice = other._logicalDevice;
        _renderPassCollection.swap(other._renderPassCollection);
    }
    return *this;
}

std::shared_ptr<RenderPass> RenderPassCollector::getOrCreateRenderPass(
    RenderTargetLayoutKey layoutKey)
{
    auto it = _renderPassCollection.find(layoutKey);

    if (it == _renderPassCollection.end())
    {
        auto renderPassCreated =
            std::make_shared<RenderPass>(_logicalDevice);
        
        if (renderPassCreated->initialize(layoutKey) == false)
        {
            renderPassCreated.reset();
            return nullptr;
        }

        _renderPassCollection.emplace(layoutKey, renderPassCreated);
        return renderPassCreated;
    }

    return it->second;
}

std::shared_ptr<FrameBuffer> RenderPassCollector::getOrCreateFrameBuffer(
    const std::shared_ptr<RenderPass>& renderPass, RenderTargetsInfo rtInfo)
{
    auto it = _frameBufferCollection.find(rtInfo);

    if (it == _frameBufferCollection.end())
    {
        auto frameBufferCreated = std::make_shared<FrameBuffer>(_logicalDevice);

        if (frameBufferCreated->initialize(renderPass, rtInfo) == false)
        {
            frameBufferCreated.reset();
            return nullptr;
        }

        _frameBufferCollection.emplace(rtInfo, frameBufferCreated);
        return frameBufferCreated;
    }

    return it->second;
}

void RenderPassCollector::release()
{
    _renderPassCollection.clear();
}

}  // namespace VoxFlow