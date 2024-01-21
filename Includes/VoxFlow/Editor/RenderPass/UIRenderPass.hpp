// Author : snowapril

#ifndef VOXEL_FLOW_UI_RENDER_PASS_HPP
#define VOXEL_FLOW_UI_RENDER_PASS_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Renderer/SceneRenderPass.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>

namespace VoxFlow
{
class LogicalDevice;
class SwapChain;
class RenderPass;

namespace RenderGraph
{
class FrameGraph;
}

class UIRenderPass : public SceneRenderPass
{
 public:
    UIRenderPass(LogicalDevice* logicalDevice, SwapChain* swapChain);
    ~UIRenderPass() override;

 public:
    bool initialize() override;
    void updateRender(ResourceUploadContext* uploadContext) override;
    void renderScene(RenderGraph::FrameGraph* frameGraph) override;

 protected:
    LogicalDevice* _logicalDevice = nullptr;
    SwapChain* _swapChain = nullptr;
    VkDescriptorPool _imguiDescriptorPool = VK_NULL_HANDLE;
    std::unique_ptr<RenderPass> _uiRenderPass;
};
}  // namespace VoxFlow

#endif