// Author : snowapril

#ifndef VOXEL_FLOW_HANDLE_HPP
#define VOXEL_FLOW_HANDLE_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <atomic>
#include <unordered_map>
#include <memory>

namespace VoxFlow
{

class HandleAllocator : private NonCopyable
{
 public:
    HandleAllocator();
    ~HandleAllocator();


 private:
    std::unordered_map<HandleBase::HandleID, void*> _handleArena;
};

}  // namespace VoxFlow

#endif