// Author : snowapril

#ifndef VOXEL_FLOW_VOX_EDITOR_HPP
#define VOXEL_FLOW_VOX_EDITOR_HPP

#include <VoxFlow/Core/Utils/DeviceInputSubscriber.hpp>

namespace VoxFlow
{
class RenderDevice;

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

private:
    void processKeyInput(DeviceKeyInputType key, const bool isReleased);

 protected:
 private:
    RenderDevice* _renderDevice = nullptr;
    DeviceInputSubscriber _inputRegistrator;
    bool _shouldCloseEditor = false;
};
}  // namespace VoxFlow

#endif