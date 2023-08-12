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

    void isReferencedByPass(PassNode* passNode);

    inline uint32_t isCulled() const
    {
        return _refCount == 0;
    }

    PassNode* getFirstReferencedPassNode() const
    {
        return _firstPass;
    }

    PassNode* getLastReferencedPassNode() const
    {
        return _lastPass;
    }

    virtual void devirtualize() {};

    virtual void destroy() {};

 protected:
    PassNode* _firstPass = nullptr;
    PassNode* _lastPass = nullptr;
    uint32_t _refCount = 0;
};

template <ResourceConcept ResourceDataType>
class Resource : public VirtualResource
{
 public:
    explicit Resource(typename ResourceDataType::Descriptor&& resourceArgs);
    ~Resource();

 public:
    inline PassNode* getProducerNode() const
    {
        return _producerPassNode;
    }

    inline typename ResourceDataType::Descriptor getDescriptor() const
    {
        return _descriptor;
    }

 protected:
    typename ResourceDataType::Descriptor _descriptor;
    PassNode* _producerPassNode = nullptr;
};

template <ResourceConcept ResourceDataType>
class ImportedResource : public Resource<ResourceDataType>
{
 public:
    ImportedResource(const ResourceDataType& resource,
                     typename ResourceDataType::Descriptor&& resourceArgs);
    ~ImportedResource();

 public:
    inline bool isImported() const final
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

    inline const std::string& getResourceName() const
    {
        return _resourceName;
    }

    inline ResourceHandle getResourceHandle() const
    {
        return _resourceHandle;
    }

 private:
    std::string _resourceName;
    ResourceHandle _resourceHandle;
};
}  // namespace FrameGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/Resource-Impl.hpp>