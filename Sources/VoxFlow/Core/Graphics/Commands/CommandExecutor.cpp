// Author : snowapril

#include <VoxFlow/Core/Graphics/Commands/CommandExecutor.hpp>

namespace VoxFlow
{
CommandExecutorBase::CommandExecutorBase(LogicalDevice* logicalDevice,
                                         CommandBufferUsage commandBufferUsage,
                                         SwapChain* swapChain)
    : _logicalDevice(logicalDevice),
      _commandBufferUsage(commandBufferUsage),
      _swapChains(swapChain)
{
    _commandPool = _logicalDevice->getOrCreateCommandPool(commandBufferUsage);
    _commandBuffer = _commandPool->getOrCreateCommandBuffer();
}
CommandExecutorBase::~CommandExecutorBase()
{
    _commandPool->releaseCommandBuffer(_commandBuffer);
}

PersistentCommandExecutor::PersistentCommandExecutor(
    LogicalDevice* logicalDevice, CommandBufferUsage commandBufferUsage,
    SwapChain* swapChain)
    : CommandExecutorBase(logicalDevice, commandBufferUsage, swapChain)
{
}
PersistentCommandExecutor::~PersistentCommandExecutor()
{
}

ScopedCommandExecutor::ScopedCommandExecutorScopedCommandExecutor(
    LogicalDevice* logicalDevice, CommandBufferUsage commandBufferUsage,
    SwapChain* swapChain)
    : CommandExecutorBase(logicalDevice, commandBufferUsage, swapChain)
{
}
ScopedCommandExecutor::~ScopedCommandExecutorScopedCommandExecutor()
{
}
}  // namespace VoxFlow
