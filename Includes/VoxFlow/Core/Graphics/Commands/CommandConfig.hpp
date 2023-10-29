// Author : snowapril

#ifndef VOXEL_FLOW_COMMAND_CONFIG_HPP
#define VOXEL_FLOW_COMMAND_CONFIG_HPP

#include <string>

namespace VoxFlow
{

constexpr const char* MAIN_GRAPHICS_STREAM_NAME = "MainGraphicsStream";
constexpr const char* ASYNC_UPLOAD_STREAM_NAME = "AsyncUploadStream";
constexpr const char* ASYNC_COMPUTE_STREAM_NAME = "AsyncComputeStream";
constexpr const char* IMMEDIATE_UPLOAD_STREAM_NAME = "ImmediateUploadStream";

enum class CommandStreamUsage : uint8_t
{
    Graphics = 0,
    Compute = 1,
    Transfer = 2,
    Count = 4,
    Undefined = 5,
};

struct CommandStreamKey
{
    std::string _cmdStreamName;
    CommandStreamUsage _cmdStreamUsage = CommandStreamUsage::Undefined;
};

inline bool operator==(const CommandStreamKey& lhs, const CommandStreamKey& rhs)
{
    return (lhs._cmdStreamName == rhs._cmdStreamName) && (lhs._cmdStreamUsage == rhs._cmdStreamUsage);
}

}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::CommandStreamKey>
{
    std::size_t operator()(VoxFlow::CommandStreamKey const& streamKey) const noexcept;
};

#endif