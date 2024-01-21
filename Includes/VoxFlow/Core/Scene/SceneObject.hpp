// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_OBJECT_HPP
#define VOXEL_FLOW_SCENE_OBJECT_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <cstdint>

namespace VoxFlow
{

enum class SceneObjectType : uint8_t
{
    Opaque = 0,
    Transparent = 1,
    Undefined = 2,
    Count = Undefined
};

class SceneObject : private NonCopyable
{
 public:
    [[nodiscard]] inline SceneObjectType getType() const
    {
        return _type;
    }

 private:
    // primitive buffer
    // material ...
    SceneObjectType _type;
};

}  // namespace VoxFlow

#endif