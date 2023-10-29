// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_DEVICE_HPP
#define VOXEL_FLOW_RENDER_DEVICE_HPP

#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraph.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <vector>

namespace VoxFlow
{
class Instance;
class PhysicalDevice;
class SceneRenderer;
class LogicalDevice;
class CommandJobSystem;
class SwapChain;

class RenderDevice final : private NonCopyable
{
 public:
    explicit RenderDevice(Context deviceSetupCtx);
    ~RenderDevice() override;

 public:
    [[nodiscard]] inline Instance* getInstance() const
    {
        return _instance;
    }

    [[nodiscard]] inline PhysicalDevice* getPhysicalDevice() const
    {
        return _physicalDevice;
    }

    [[nodiscard]] inline LogicalDevice* getLogicalDevice(const LogicalDeviceType deviceType) const
    {
        return _logicalDevices.at(static_cast<uint32_t>(deviceType)).get();
    }

    [[nodiscard]] inline SceneRenderer* getSceneRenderer() const
    {
        return _sceneRenderer.get();
    }

    /**
     * @return render resource upload context which is dedicated to logical
     * device
     */
    [[nodiscard]] ResourceUploadContext* getResourceUploadContext() const
    {
        return _uploadContext;
    }

 public:
    void initializePasses();
    void updateRender(const double deltaTime);
    void renderScene();

 private:
    void release();
    void waitForRenderReady(const uint32_t frameIndex);

 protected:
 private:
    Instance* _instance = nullptr;
    PhysicalDevice* _physicalDevice = nullptr;
    std::vector<std::unique_ptr<LogicalDevice>> _logicalDevices;
    std::unique_ptr<SceneRenderer> _sceneRenderer;
    Context* _deviceSetupCtx = nullptr;
    RenderGraph::FrameGraph _frameGraph;
    FrameContext _frameContext;
    std::shared_ptr<SwapChain> _mainSwapChain;
    CommandJobSystem* _mainCmdJobSystem = nullptr;
    ResourceUploadContext* _uploadContext = nullptr;
};
}  // namespace VoxFlow

#endif