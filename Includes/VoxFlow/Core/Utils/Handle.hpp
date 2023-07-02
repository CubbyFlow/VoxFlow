// Author : snowapril

#ifndef VOXEL_FLOW_HANDLE_HPP
#define VOXEL_FLOW_HANDLE_HPP

#include <VoxFlow/Core/Utils/RendererCommon.hpp>

namespace VoxFlow
{

class HandleBase
{
 public:
    using HandleId = uint32_t;

 protected:
    HandleId _handleId;
};

template <typename Type>
class Handle : public HandleBase
{
 public:
};

}  // namespace VoxFlow

#endif