// Author : snowapril

#ifndef VOX_FLOW_HPP
#define VOX_FLOW_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <Core/CUDA/CUDAArray.hpp>
#include <Core/CUDA/CUDAPCISPHSolver3.hpp>

namespace VoxFlow
{
class VoxFlow
{
 public:
    VoxFlow();
    ~VoxFlow();

 public:
    bool initialize();

    void updateSolver();

 private:
    std::shared_ptr<CUDAPCISPHSolver3> _sphSolver = nullptr;
};
}  // namespace VoxFlow

#endif