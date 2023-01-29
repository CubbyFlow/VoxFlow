// Author : snowapril

#ifndef VOXEL_FLOW_VOX_ENGINE_HPP
#define VOXEL_FLOW_VOX_ENGINE_HPP

namespace VoxFlow
{
class RenderDevice;

class VoxEngine
{
 public:
    VoxEngine();
    ~VoxEngine();

 protected:
 private:
    RenderDevice* _renderDevice = nullptr;
};
}  // namespace VoxFlow

#endif