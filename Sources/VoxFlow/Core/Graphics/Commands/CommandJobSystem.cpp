// Author : snowapril

#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>

namespace VoxFlow
{
CommandJobSystem::CommandJobSystem(LogicalDevice* logicalDevice)
    : _logicalDevice(logicalDevice)
{
}

CommandJobSystem::~CommandJobSystem()
{
}

}  // namespace VoxFlow
