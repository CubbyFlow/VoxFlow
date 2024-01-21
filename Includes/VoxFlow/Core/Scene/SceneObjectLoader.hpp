// Author : snowapril

#ifndef VOXEL_FLOW_SCENE_OBJECT_LOADER_HPP
#define VOXEL_FLOW_SCENE_OBJECT_LOADER_HPP

#include <filesystem>
#include <glm/vec2.hpp>

namespace VoxFlow
{

class SceneObjectCollection;
class Texture;
class CommandStream;
class ResourceUploadContext;

struct StaticMeshInfo
{

};

struct TextureUploadInfo
{
    Texture* uploadDstTexture = nullptr;
    void* textureRawData = nullptr;
    glm::vec2 extent;
    int numChannels;
    size_t numBytes;
};

class SceneObjectLoader
{
 public:
    SceneObjectLoader() = default;
    ~SceneObjectLoader() = default;

    // Blocking
    bool loadSceneObject(const std::filesystem::path& gltfPath, ResourceUploadContext* asyncUploadContext, CommandStream* asyncCmdStream,
                         SceneObjectCollection* outCollection);
};

}  // namespace VoxFlow

#endif