// Author : snowapril

#include <VoxFlow/Editor/RenderPass/SDFGlobalIlluminationPass.hpp>
#include <VoxFlow/Core/Devices/LogicalDevice.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Resources/ResourceUploadContext.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>

namespace VoxFlow
{

SDFGlobalIlluminationPass::SDFGlobalIlluminationPass(LogicalDevice* logicalDevice) : _logicalDevice(logicalDevice)
{
}

SDFGlobalIlluminationPass::~SDFGlobalIlluminationPass()
{
}

bool SDFGlobalIlluminationPass::initialize()
{
    return true;
}

void SDFGlobalIlluminationPass::updateRender(ResourceUploadContext* uploadContext)
{
    (void)uploadContext;
}

void SDFGlobalIlluminationPass::renderScene(RenderGraph::FrameGraph* frameGraph)
{
    (void)frameGraph;
}

}  // namespace VoxFlow