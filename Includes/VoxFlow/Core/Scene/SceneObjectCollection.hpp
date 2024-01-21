// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_OBJECT_COLLECTION_HPP
#define VOXEL_FLOW_SCENE_OBJECT_COLLECTION_HPP

#include <VoxFlow/Core/Scene/SceneObject.hpp>
#include <array>
#include <vector>
#include <memory>

namespace VoxFlow
{

class SceneObject;

class SceneObjectCollection
{
 public:
    SceneObjectCollection() = default;
    ~SceneObjectCollection() = default;

 private:
    SceneObjectType _sceneObjectType;
    std::vector<std::unique_ptr<SceneObject>> _sceneObjects;
};

}  // namespace VoxFlow

#endif