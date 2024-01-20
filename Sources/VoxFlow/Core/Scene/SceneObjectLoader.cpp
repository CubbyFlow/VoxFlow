// Author : snowapril

#include <VoxFlow/Core/Scene/Material.hpp>
#include <VoxFlow/Core/Scene/SceneObjectCollection.hpp>
#include <VoxFlow/Core/Scene/SceneObjectLoader.hpp>
#include <VoxFlow/Core/Utils/Logger.hpp>
#include <variant>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include <fastgltf/parser.hpp>
#include <fastgltf/types.hpp>

#include <VoxFlow/Core/Resources/Texture.hpp>

namespace VoxFlow
{
bool SceneObjectLoader::loadSceneObject(const std::filesystem::path& gltfPath, SceneObjectCollection* outCollection)
{
    if (std::filesystem::exists(gltfPath) == false)
    {
        VOX_ASSERT(false, "Failed to load gltf path : {}", gltfPath.generic_string());
        return false;
    }

    fastgltf::Parser parser(fastgltf::Extensions::KHR_mesh_quantization);

    constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember | fastgltf::Options::AllowDouble | fastgltf::Options::LoadGLBBuffers |
                                 fastgltf::Options::LoadExternalBuffers | fastgltf::Options::LoadExternalImages | fastgltf::Options::GenerateMeshIndices;

    fastgltf::GltfDataBuffer data;
    data.loadFromFile(gltfPath);

    auto asset = parser.loadGLTF(&data, gltfPath.parent_path(), gltfOptions);

    if (asset.error() != fastgltf::Error::None)
    {
        VOX_ASSERT(false, "Failed to load gltf path : {}, error : {}", gltfPath.generic_string(), fastgltf::getErrorMessage(asset.error()));
        return false;
    }

    std::vector<fastgltf::sources::Vector> buffers;
    std::vector<std::unique_ptr<Texture>> textures;
    std::vector<std::unique_ptr<Material>> materials;
    std::vector<TextureUploadInfo> textureUploadInfos;

    for (auto& buffer : asset->buffers)
    {
        std::visit(fastgltf::visitor{
                       [](auto& arg) { (void)arg; },  // Covers FilePathWithOffset, BufferView, ... which are all not possible
                       [&](fastgltf::sources::Vector& vector) { buffers.emplace_back(vector); },
                       [&](fastgltf::sources::CustomBuffer& customBuffer) {
                           // We don't need to do anything special here, the buffer has already been created.
                           // viewer->buffers.emplace_back(static_cast<GLuint>(customBuffer.id));
                           (void)customBuffer;
                       },
                   },
                   buffer.data);
    }

    for (const auto& image : asset->images)
    {
        std::visit(
            [&textureUploadInfos, &asset](auto&& arg) { 
                using T = std::decay_t<decltype(arg)>;
                if constexpr (std::is_same_v < T, fastgltf::sources::URI>)
                {
                    VOX_ASSERT(arg.fileByteOffset == 0, "We don't support offsets with stbi");
                    VOX_ASSERT(arg.uri.isLocalPath(), "We're only capable of loading local files");
                    int width, height, nrChannels;

                    const std::string path(arg.uri.path().begin(), arg.uri.path().end());  // Thanks C++.
                    unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrChannels, 4);

                    textureUploadInfos.push_back(
                        { .uploadDstTexture = nullptr, .textureRawData = data, .extent = glm::vec2(width, height), .numChannels = nrChannels });
                }
                else if constexpr (std::is_same_v<T, fastgltf::sources::Vector>)
                {
                    int width, height, nrChannels;
                    unsigned char* data = stbi_load_from_memory(arg.bytes.data(), static_cast<int>(arg.bytes.size()), &width, &height, &nrChannels, 4);

                    textureUploadInfos.push_back(
                        { .uploadDstTexture = nullptr, .textureRawData = data, .extent = glm::vec2(width, height), .numChannels = nrChannels });
                }
                else if constexpr (std::is_same_v<T, fastgltf::sources::BufferView>)
                {
                    auto& bufferView = asset->bufferViews[arg.bufferViewIndex];
                    auto& buffer = asset->buffers[bufferView.bufferIndex];
                    // Yes, we've already loaded every buffer into some GL buffer. However, with GL it's simpler
                    // to just copy the buffer data again for the texture. Besides, this is just an example.
                    std::visit(
                        fastgltf::visitor{
                            // We only care about VectorWithMime here, because we specify LoadExternalBuffers, meaning
                            // all buffers are already loaded into a vector.
                            [](auto& arg) { (void)arg; },
                            [&](fastgltf::sources::Vector& vector) {
                                int width, height, nrChannels;
                                unsigned char* data = stbi_load_from_memory(vector.bytes.data() + bufferView.byteOffset,
                                                                            static_cast<int>(bufferView.byteLength), &width, &height, &nrChannels, 4);

                                textureUploadInfos.push_back(
                                    { .uploadDstTexture = nullptr, .textureRawData = data, .extent = glm::vec2(width, height), .numChannels = nrChannels });
                            } },
                        buffer.data);
                }
                else
                {
                    VOX_ASSERT(false, "Unknown data type");
                }
            },
            image.data);
    }

    //for (const auto& material : asset->materials)
    //{
    //    materials.push_back(std::make_unique<Material>(std::make_unique<MaterialBaseColor>());
    //}

    //for (const auto& mesh : asset->meshes)
    //{
    //    for (auto it = mesh.primitives.begin(); it != mesh.primitives.end(); ++it)
    //    {
    //        auto* positionIt = it->findAttribute("POSITION");
    //        // A mesh primitive is required to hold the POSITION attribute.
    //        assert(positionIt != it->attributes.end());

    //        // We only support indexed geometry.
    //        if (!it->indicesAccessor.has_value())
    //        {
    //            return false;
    //        }

    //        // Get the output primitive
    //        auto index = std::distance(mesh.primitives.begin(), it);
    //        auto& primitive = outMesh.primitives[index];
    //        primitive.primitiveType = fastgltf::to_underlying(it->type);
    //        if (it->materialIndex.has_value())
    //        {
    //            primitive.materialUniformsIndex = it->materialIndex.value() + 1;  // Adjust for default material
    //            auto& material = asset->materials[it->materialIndex.value()];
    //            if (material.pbrData.baseColorTexture.has_value())
    //            {
    //                auto& texture = asset->textures[material.pbrData.baseColorTexture->textureIndex];
    //                if (!texture.imageIndex.has_value())
    //                    return false;
    //                primitive.albedoTexture = viewer->textures[texture.imageIndex.value()].texture;
    //            }
    //        }
    //        else
    //        {
    //            primitive.materialUniformsIndex = 0;
    //        }

    //        {
    //            // Position
    //            auto& positionAccessor = asset.accessors[positionIt->second];
    //            if (!positionAccessor.bufferViewIndex.has_value())
    //                continue;

    //            glEnableVertexArrayAttrib(vao, 0);
    //            glVertexArrayAttribFormat(vao, 0, static_cast<GLint>(fastgltf::getNumComponents(positionAccessor.type)),
    //                                      fastgltf::getGLComponentType(positionAccessor.componentType), GL_FALSE, 0);
    //            glVertexArrayAttribBinding(vao, 0, 0);

    //            auto& positionView = asset.bufferViews[positionAccessor.bufferViewIndex.value()];
    //            auto offset = positionView.byteOffset + positionAccessor.byteOffset;
    //            if (positionView.byteStride.has_value())
    //            {
    //                glVertexArrayVertexBuffer(vao, 0, viewer->buffers[positionView.bufferIndex], static_cast<GLintptr>(offset),
    //                                          static_cast<GLsizei>(positionView.byteStride.value()));
    //            }
    //            else
    //            {
    //                glVertexArrayVertexBuffer(vao, 0, viewer->buffers[positionView.bufferIndex], static_cast<GLintptr>(offset),
    //                                          static_cast<GLsizei>(fastgltf::getElementByteSize(positionAccessor.type, positionAccessor.componentType)));
    //            }
    //        }

    //        if (const auto* texcoord0 = it->findAttribute("TEXCOORD_0"); texcoord0 != it->attributes.end())
    //        {
    //            // Tex coord
    //            auto& texCoordAccessor = asset.accessors[texcoord0->second];
    //            if (!texCoordAccessor.bufferViewIndex.has_value())
    //                continue;

    //            glEnableVertexArrayAttrib(vao, 1);
    //            glVertexArrayAttribFormat(vao, 1, static_cast<GLint>(fastgltf::getNumComponents(texCoordAccessor.type)),
    //                                      fastgltf::getGLComponentType(texCoordAccessor.componentType), GL_FALSE, 0);
    //            glVertexArrayAttribBinding(vao, 1, 1);

    //            auto& texCoordView = asset.bufferViews[texCoordAccessor.bufferViewIndex.value()];
    //            auto offset = texCoordView.byteOffset + texCoordAccessor.byteOffset;
    //            if (texCoordView.byteStride.has_value())
    //            {
    //                glVertexArrayVertexBuffer(vao, 1, viewer->buffers[texCoordView.bufferIndex], static_cast<GLintptr>(offset),
    //                                          static_cast<GLsizei>(texCoordView.byteStride.value()));
    //            }
    //            else
    //            {
    //                glVertexArrayVertexBuffer(vao, 1, viewer->buffers[texCoordView.bufferIndex], static_cast<GLintptr>(offset),
    //                                          static_cast<GLsizei>(fastgltf::getElementByteSize(texCoordAccessor.type, texCoordAccessor.componentType)));
    //            }
    //        }

    //        // Generate the indirect draw command
    //        auto& draw = primitive.draw;
    //        draw.instanceCount = 1;
    //        draw.baseInstance = 0;
    //        draw.baseVertex = 0;

    //        auto& indices = asset.accessors[it->indicesAccessor.value()];
    //        if (!indices.bufferViewIndex.has_value())
    //            return false;
    //        draw.count = static_cast<uint32_t>(indices.count);

    //        auto& indicesView = asset.bufferViews[indices.bufferViewIndex.value()];
    //        draw.firstIndex =
    //            static_cast<uint32_t>(indices.byteOffset + indicesView.byteOffset) / fastgltf::getElementByteSize(indices.type, indices.componentType);
    //        primitive.indexType = getGLComponentType(indices.componentType);
    //        glVertexArrayElementBuffer(vao, viewer->buffers[indicesView.bufferIndex]);
    //    }
    //}

    (void)outCollection;
    return true;
};

}  // namespace VoxFlow