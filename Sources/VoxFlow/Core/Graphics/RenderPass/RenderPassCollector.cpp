// Author : snowapril

#include <VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp>
#include <VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp>

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

    if (it != _renderPassCollection.end())
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

void RenderPassCollector::release()
{
    _renderPassCollection.clear();
}

}  // namespace VoxFlow