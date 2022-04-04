// Author : snowapril

#ifndef VOXEL_FLOW_NONCOPYABLE_HPP
#define VOXEL_FLOW_NONCOPYABLE_HPP

namespace VoxFlow
{
class NonCopyable
{
 public:
    NonCopyable() = default;
    virtual ~NonCopyable() = default;
    NonCopyable(const NonCopyable&) = delete;
    NonCopyable& operator=(const NonCopyable&) = delete;
};
}  // namespace VoxelFlow

#endif