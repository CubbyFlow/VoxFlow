// Author : snowapril

#ifndef VOXEL_FLOW_RENDER_DEVICE_HPP
#define VOXEL_FLOW_RENDER_DEVICE_HPP

#include <VoxFlow/Core/Devices/Context.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <vector>

namespace VoxFlow
{
class Instance;
class PhysicalDevice;
class LogicalDevice;
class Queue;
class SwapChain;
class CommandPool;
class CommandBuffer;
class GraphicsPipeline;
class ShaderModule;
class PipelineLayout;

class RenderDevice : private NonCopyable
{
 public:
    explicit RenderDevice(Context deviceSetupCtx);
    ~RenderDevice();

 public:
    [[nodiscard]] inline Instance* getInstance() const
    {
        return _instance;
    }
    [[nodiscard]] inline PhysicalDevice* getPhysicalDevice() const
    {
        return _physicalDevice;
    }
    [[nodiscard]] inline LogicalDevice* getLogicalDevice(
        const uint32_t deviceIndex) const
    {
        VOX_ASSERT(deviceIndex < _logicalDevices.size(),
                   "Given Index({}), Num LogicalDevices({})", deviceIndex,
                   _logicalDevices.size());
        return _logicalDevices.at(deviceIndex);
    }

public:
    void beginFrame(const uint32_t deviceIndex);
    void presentSwapChains(const uint32_t deviceIndex);

 protected:
 private:
    Instance* _instance = nullptr;
    PhysicalDevice* _physicalDevice = nullptr;
    std::vector<LogicalDevice*> _logicalDevices;
    Context* _deviceSetupCtx = nullptr;
    
    // TODO(snowapril) : to be removed
    Queue* _mainQueue = nullptr;
    CommandPool* _mainCommandPool = nullptr;
    std::vector<std::shared_ptr<CommandBuffer>> _mainCommandBuffers;
    std::shared_ptr<GraphicsPipeline> _trianglePipeline = nullptr;
    
};
}  // namespace VoxFlow

#endif