// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_EXECUTOR_HPP
#define VOXEL_FLOW_COMMAND_EXECUTOR_HPP

#include <volk/volk.h>
#include <VoxFlow/Core/Utils/FenceObject.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <VoxFlow/Core/Utils/RendererCommon.hpp>
#include <string>
#include <memory>

namespace VoxFlow
{
class CommandPool;
class LogicalDevice;
class CommandBuffer;
class SwapChain;

class CommandExecutorBase : private NonCopyable
{
 public:
    explicit CommandExecutorBase(LogicalDevice* logicalDevice,
                                 CommandBufferUsage commandBufferUsage,
                                 SwapChain* swapChain);
    virtual ~CommandExecutorBase();

    inline CommandBufferUsage getCommandBufferUsage() const
    {
        return _commandBufferUsage;
    }

 protected:
    LogicalDevice* _logicalDevlce = nullptr;
    CommandBufferUsage _commandBufferUsage = CommandBufferUsage::Undefined;
    std::shared_ptr<CommandBuffer> _commandBuffer = nullptr;
    CommandPool* _ownerCommandPool = nullptr;
    SwapChain* _swapChain = nullptr;
};

class PersistentCommandExecutor : public CommandExecutorBase
{
 public:
    explicit PersistentCommandExecutor(LogicalDevice* logicalDevice,
                                       CommandBufferUsage commandBufferUsage,
                                       SwapChain* swapChain);
    ~PersistentCommandExecutor();
};

class ScopedCommandExecutor : public CommandExecutorBase
{
 public:
    explicit ScopedCommandExecutor(LogicalDevice* logicalDevice,
                                   CommandBufferUsage commandBufferUsage,
                                   SwapChain* swapChain);
    ~ScopedCommandExecutor();
};

}  // namespace VoxFlow

#endif