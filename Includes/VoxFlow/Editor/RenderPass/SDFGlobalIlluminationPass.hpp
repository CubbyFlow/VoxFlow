// Author : snowapril

#ifndef VOXEL_FLOW_SDF_GLOBAL_ILLUMINATION_PASS_HPP
#define VOXEL_FLOW_SDF_GLOBAL_ILLUMINATION_PASS_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>

namespace VoxFlow
{
class RenderDevice;
class Buffer;
class LogicalDevice;

namespace RenderGraph
{
class FrameGraph;
}

class SDFGlobalIlluminationPass : public SceneRenderPass
{
 public:
    SDFGlobalIlluminationPass(LogicalDevice* logicalDevice);
    ~SDFGlobalIlluminationPass() override;

 public:
    bool initialize() override;
    void updateRender(ResourceUploadContext* uploadContext) override;
    void renderScene(RenderGraph::FrameGraph* frameGraph) override;

 protected:
    LogicalDevice* _logicalDevice = nullptr;
};
}  // namespace VoxFlow

#endif