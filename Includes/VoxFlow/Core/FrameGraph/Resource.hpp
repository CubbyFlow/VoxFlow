// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCE_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCE_HPP

#include <VoxFlow/Core/FrameGraph/DependencyGraph.hpp>
#include <VoxFlow/Core/FrameGraph/TypeTraits.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphTexture.hpp>
#include <VoxFlow/Core/FrameGraph/FrameGraphRenderPass.hpp>
#include <VoxFlow/Core/FrameGraph/ResourceHandle.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>
#include <string_view>

namespace VoxFlow
{
class Texture;

namespace RenderGraph
{
class PassNode;

class ResourceNode : public DependencyGraph::Node
{
 public:
    explicit ResourceNode(DependencyGraph* dependencyGraph,
                          ResourceHandle resourceHandle);

    inline ResourceHandle getResourceHandle() const
    {
        return _resourceHandle;
    }

 private:
    ResourceHandle _resourceHandle;
};

class VirtualResource : private NonCopyable
{
 public:
    VirtualResource(std::string&& name);
    virtual ~VirtualResource();

    virtual bool isImported() const
    {
        return false;
    }

    void isReferencedByPass(PassNode* passNode);

    [[nodiscard]] inline uint32_t isCulled() const
    {
        return _refCount == 0;
    }

    [[nodiscard]] inline PassNode* getFirstReferencedPassNode() const
    {
        return _firstPass;
    }

    [[nodiscard]] inline PassNode* getLastReferencedPassNode() const
    {
        return _lastPass;
    }

    [[nodiscard]] inline const std::string& getResourceName() const
    {
        return _resourceName;
    }

    virtual void devirtualize(RenderResourceAllocator*) = 0;

    virtual void destroy(RenderResourceAllocator*) = 0;

 protected:
    std::string _resourceName;
    PassNode* _firstPass = nullptr;
    PassNode* _lastPass = nullptr;
    uint32_t _refCount = 0;
};

template <ResourceConcept ResourceDataType>
class Resource : public VirtualResource
{
 public:
    explicit Resource(std::string&& name,
                      typename ResourceDataType::Descriptor&& resourceArgs);
    explicit Resource(std::string&& name,
                      typename ResourceDataType::Descriptor&& resourceArgs,
                      const ResourceDataType& resource);
    explicit Resource(std::string&& name,
                      typename ResourceDataType::Descriptor&& resourceArgs,
                      typename ResourceDataType::Usage usage);
    explicit Resource(std::string&& name,
                      typename ResourceDataType::Descriptor&& resourceArgs,
                      typename ResourceDataType::Usage usage,
                      const ResourceDataType& resource);
    ~Resource() = default;

 public:
    [[nodiscard]] inline PassNode* getProducerNode() const
    {
        return _producerPassNode;
    }

    [[nodiscard]] inline const ResourceDataType& getInternalResource() const
    {
        return _resource;
    }

    [[nodiscard]] inline typename ResourceDataType::Descriptor getDescriptor() const
    {
        return _descriptor;
    }

    void devirtualize(RenderResourceAllocator* allocator) override
    {
        _resource.create(allocator, std::move(_resourceName), _descriptor, _usage);
    }

    void destroy(RenderResourceAllocator* allocator) override
    {
        _resource.destroy(allocator);
    }

 public:
    class ResourceEdge : public DependencyGraph::Edge
    {
     public:
        explicit ResourceEdge(DependencyGraph* ownerGraph,
                              DependencyGraph::Node* from,
                              DependencyGraph::Node* to,
                              typename ResourceDataType::Usage usage)
            : DependencyGraph::Edge(ownerGraph, from, to), _usage(usage)
        {
        }

        inline typename ResourceDataType::Usage getUsage() const noexcept
        {
            return _usage;
        }

        ResourceEdge& operator|=(typename ResourceDataType::Usage usage)
        {
            _usage |= usage;
            return *this;
        }

     private:
        typename ResourceDataType::Usage _usage;
    };

    bool connect(DependencyGraph* dependencyGraph, ResourceNode* node,
                 PassNode* passNode, typename ResourceDataType::Usage usage)
    {
        // TODO(snowapril) : add edges
        (void)dependencyGraph;
        (void)passNode;
        (void)node;
        (void)usage;
        return true;
    }

    bool connect(DependencyGraph* dependencyGraph, PassNode* passNode,
                 ResourceNode* node, typename ResourceDataType::Usage usage)
    {
        // TODO(snowapril) : add edges
        (void)dependencyGraph;
        (void)passNode;
        (void)node;
        (void)usage;
        return true;
    }

 protected:
    typename ResourceDataType::Descriptor _descriptor;
    ResourceDataType _resource;
    typename ResourceDataType::Usage _usage;
    PassNode* _producerPassNode = nullptr;
};

template <ResourceConcept ResourceDataType>
class ImportedResource : public Resource<ResourceDataType>
{
 public:
    ImportedResource(std::string&& name,
                     typename ResourceDataType::Descriptor&& resourceArgs,
                     typename ResourceDataType::Usage usage,
                     const ResourceDataType& resource)
        : Resource<ResourceDataType>(std::move(name),
                                     std::move(resourceArgs), usage, resource)
    {
    }
    ~ImportedResource()
    {
    }

 public:
    inline bool isImported() const final
    {
        return true;
    }

    void devirtualize(RenderResourceAllocator*) final {};

    void destroy(RenderResourceAllocator*) final {};
};

class ImportedRenderTarget : public ImportedResource<FrameGraphTexture>
{
 public:
    ImportedRenderTarget(std::string&& name,
                         FrameGraphTexture::Descriptor&& resourceArgs,
                         FrameGraphRenderPass::ImportedDescriptor&& importedDesc,
                         const FrameGraphTexture& resource,
                         TextureView* textureView);
    ~ImportedRenderTarget();

    inline TextureView* getTextureView()
    {
        return _textureViewHandle;
    }

 protected:
 private:
    TextureView* _textureViewHandle = nullptr;
};
}  // namespace RenderGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/Resource-Impl.hpp>