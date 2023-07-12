// Author : snowapril

#ifndef VOXEL_FLOW_VOX_EDITOR_HPP
#define VOXEL_FLOW_VOX_EDITOR_HPP

#include <VoxFlow/Core/Utils/DeviceInputSubscriber.hpp>

namespace VoxFlow
{
class RenderDevice;
class SceneObjectPass;
class PostProcessPass;

class VoxEditor
{
 public:
    VoxEditor();
    ~VoxEditor();

 public:
    void runEditorLoop();

private:
    void processKeyInput(DeviceKeyInputType key, const bool isReleased);

 protected:
 private:
    RenderDevice* _renderDevice = nullptr;
    DeviceInputSubscriber _inputRegistrator;
    bool _shouldCloseEditor = false;

    SceneObjectPass* _sceneObjectPass = nullptr;
    PostProcessPass* _postProcessPass = nullptr;
};
}  // namespace VoxFlow

#endif