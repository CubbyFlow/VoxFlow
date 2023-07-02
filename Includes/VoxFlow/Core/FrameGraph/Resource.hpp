// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCE_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCE_HPP

#include <VoxFlow/Core/FrameGraph/DependencyGraph.hpp>
#include <VoxFlow/Core/FrameGraph/TypeTraits.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <memory>

namespace VoxFlow
{
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

 protected:
};

template <ResourceConcept ResourceDataType>
class Resource : public VirtualResource
{
 public:
    Resource(ResourceDataType::Descriptor&& resourceArgs);

    Resource(const ResourceDataType& resource,
                       ResourceDataType::Descriptor&& resourceArgs);

    Resource(Resource<ResourceDataType>&& rhs);
    Resource& operator=(Resource<ResourceDataType>&& rhs);

    ~Resource();

 public:
    inline PassNode* getProducerNode() const
    {
        return _producerPassNode;
    }

    inline bool isImported() const
    {
        return _isImportedResource;
    }

 protected:
 private:
    ResourceDataType _resource;
    ResourceDataType::Descriptor _descriptor;
    PassNode* _producerPassNode = nullptr;
    bool _isImportedResource = false;
};

class ResourceNode : public DependencyGraph::Node
{
};
}  // namespace FrameGraph
}  // namespace VoxFlow

#endif

#include <VoxFlow/Core/FrameGraph/Resource-Impl.hpp>