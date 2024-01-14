// Author : snowapril

#ifndef VOXEL_FLOW_MATERIAL_HPP
#define VOXEL_FLOW_MATERIAL_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <glm/vec4.hpp>
#include <memory>

namespace VoxFlow
{
class TextureView;

struct MaterialUniformBase
{
};

struct MaterialBaseColor : public MaterialUniformBase
{
    float alphaCutOff = 0.0f;
    glm::vec4 baseColorFactor;
    TextureView* baseColorView = nullptr;

    MaterialBaseColor(float alphaCutOff, glm::vec4 baseColorFactor, TextureView* baseColorView)
        : alphaCutOff(alphaCutOff), baseColorFactor(baseColorFactor), baseColorView(baseColorView)
    {

    }
};

class Material : private NonCopyable
{
 public:
    Material(std::unique_ptr<MaterialUniformBase>&& materialUniform) : _materialUniform(std::move(materialUniform))
    {
    }
    ~Material() = default;

 private:
    std::unique_ptr<MaterialUniformBase> _materialUniform;
};

}  // namespace VoxFlow

#endif