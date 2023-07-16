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

class CommandJobSystem final : private NonCopyable
{
 public:
    explicit CommandJobSystem(LogicalDevice* logicalDevice);
    ~CommandJobSystem() override;

 protected:
    LogicalDevice* _logicalDevice = nullptr;
};

}  // namespace VoxFlow

#endif