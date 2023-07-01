// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCE_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCE_HPP

#include <VoxFlow/Core/FrameGraph/DependencyGraph.hpp>
#include <VoxFlow/Core/FrameGraph/TypeTraits.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>
#include <string_view>

namespace VoxFlow
{
class Texture;

namespace FrameGraph
{
class PassNode;

using ResourceHandle = uint32_t;
constexpr ResourceHandle InvalidFrameGraphResource = UINT32_MAX;

class VirtualResource : private NonCopyable
{
 public:
    VirtualResource();
    virtual ~VirtualResource();

    virtual bool isImported() const
    {
        return false;
    }

 protected:
};

template <ResourceConcept ResourceDataType>
class Resource : public VirtualResource
{
 public:
    Resource(ResourceDataType::Descriptor&& resourceArgs);
    ~Resource();

 public:
    inline PassNode* getProducerNode() const
    {
        return _producerPassNode;
    }

 protected:
    ResourceDataType::Descriptor _descriptor;
    PassNode* _producerPassNode = nullptr;
};

template <ResourceConcept ResourceDataType>
class ImportedResource : public Resource<ResourceDataType>
{
 public:
    ImportedResource(const ResourceDataType& resource,
             ResourceDataType::Descriptor&& resourceArgs);
    ~ImportedResource();

 public:

    inline bool isImported() const override final
    {
        return true;
    }

 protected:
    ResourceDataType _resource;
};

class ImportedRenderTarget : public ImportedResource<FrameGraphTexture>
{
 public:
    ImportedRenderTarget(const FrameGraphTexture& resource,
                         FrameGraphTexture::Descriptor&& resourceArgs,
                         Texture* texture);
    ~ImportedRenderTarget();

 protected:
 private:
    Texture* _textureHandle = nullptr;
};

class ResourceNode : public DependencyGraph::Node
{
 public:
    explicit ResourceNode(DependencyGraph* dependencyGraph,
                          std::string_view&& resourceName,
        ResourceHandle resourceHandle);

 private:
    std::string _resourceName;
    ResourceHandle _resourceHandle;
};
}  // namespace FrameGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/Resource-Impl.hpp>