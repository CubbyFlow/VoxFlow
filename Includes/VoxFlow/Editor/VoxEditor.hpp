// Author : snowapril

#ifndef VOXEL_FLOW_VOX_EDITOR_HPP
#define VOXEL_FLOW_VOX_EDITOR_HPP

namespace VoxFlow
{
class VoxEngine;

class VoxEditor
{
 public:
    VoxEditor();
    ~VoxEditor();

 public:
    void runEditorLoop();

 protected:
    void processInput();
    void preUpdateFrame();
    void updateFrame();
    void renderFrame();
    void postRenderFrame();

 protected:
 private:
    VoxEngine* _voxEngine = nullptr;
};
}  // namespace VoxFlow

#endif