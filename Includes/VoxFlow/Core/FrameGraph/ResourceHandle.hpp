// Author : snowapril

#ifndef VOXEL_FLOW_FRAME_GRAPH_RESOURCE_HANDLE_HPP
#define VOXEL_FLOW_FRAME_GRAPH_RESOURCE_HANDLE_HPP

#include <VoxFlow/Core/Utils/HashUtil.hpp>

namespace VoxFlow
{

namespace RenderGraph
{
constexpr const uint32_t INVALID_HANDLE_ID = UINT32_MAX;

class ResourceHandle
{
 public:
    ResourceHandle() : _handleID(INVALID_HANDLE_ID){}
    constexpr ResourceHandle(const uint32_t id) noexcept : _handleID(id)
    {
    }
    constexpr ResourceHandle(const std::size_t id) noexcept
        : _handleID(static_cast<uint32_t>(id))
    {
    }

    operator bool() const noexcept
    {
        return _handleID != INVALID_HANDLE_ID;
    }

    inline bool operator==(const ResourceHandle& rhs) const noexcept
    {
        return _handleID == rhs._handleID;
    }

    inline bool operator<(const ResourceHandle& rhs) const noexcept
    {
        return _handleID < rhs._handleID;
    }

    inline bool operator>(const ResourceHandle& rhs) const noexcept
    {
        return _handleID > rhs._handleID;
    }

    inline operator uint32_t() const noexcept
    {
        return _handleID;
    }

    inline uint32_t get() const noexcept
    {
        return _handleID;
    }

    inline void reset() noexcept
    {
        _handleID = INVALID_HANDLE_ID;
    }

 private:
    uint32_t _handleID = INVALID_HANDLE_ID;
};

constexpr const ResourceHandle INVALID_RESOURCE_HANDLE(INVALID_HANDLE_ID);
}  // namespace RenderGraph

}  // namespace VoxFlow

template <>
struct std::hash<VoxFlow::RenderGraph::ResourceHandle>
{
    std::size_t operator()(
        VoxFlow::RenderGraph::ResourceHandle const& handle) const noexcept;
};

#endif