// Author : snowapril

#ifndef VOX_FLOW_SCENE_OBJECT_LOADER_HPP
#define VOX_FLOW_SCENE_OBJECT_LOADER_HPP

#include <volk/volk.h>
#include <memory>
#include <string>

namespace VoxFlow
{
class SceneObject;
class ResourceUploadContext;

class SceneObjectLoader
{
 public:
    SceneObjectLoader();
    ~SceneObjectLoader();

 public:
    std::shared_ptr<SceneObject> loadSceneObjectGltf(const std::string& objPath, ResourceUploadContext* uploadContext);
};
}  // namespace VoxFlow

#endif  // VOX_FLOW_SCENE_OBJECT_LOADER_HPP