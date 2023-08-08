// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_JOB_SYSTEM_IMPL_HPP
#define VOXEL_FLOW_COMMAND_JOB_SYSTEM_IMPL_HPP

#include <VoxFlow/Core/Graphics/Commands/CommandJobSystem.hpp>
#include <VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp>

namespace VoxFlow
{
template <typename... CommandJobArgs>
void CommandStream::addJob(CommandJobType jobType, CommandJobArgs... args)
{
    CommandBuffer* cmdBuffer = getOrAllocateCommandBuffer();

    switch (jobType)
    {
        case CommandJobType::BindPipeline:
            cmdBuffer.bindPipeline(args...);
            break;
        case CommandJobType::SetViewport:
            cmdBuffer.setViewport(args...);
            break;
        case CommandJobType::BindResourceGroup:
            cmdBuffer.bindResourceGroup(args...);
            break;
        case CommandJobType::UploadBuffer:
            cmdBuffer.uploadBuffer(args...);
            break;
        case CommandJobType::UploadTexture:
            cmdBuffer.uploadTexture(args...);
            break;
        case CommandJobType::DrawIndexed:
            cmdBuffer.drawIndexed(args...);
            break;
    }
}
}  // namespace VoxFlow

#endif