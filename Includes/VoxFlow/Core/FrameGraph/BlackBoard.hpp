// Author : snowapril

#ifndef VOXEL_FLOW_BLACK_BOARD_HPP
#define VOXEL_FLOW_BLACK_BOARD_HPP

#include <VoxFlow/Core/FrameGraph/Resource.hpp>
#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <string_view>
#include <unordered_map>

namespace VoxFlow
{
namespace FrameGraph
{
class BlackBoard : private NonCopyable
{
 public:
    using KeyType = std::string_view;
    using ValueType = ResourceHandle;
    using ContainerType = std::unordered_map<KeyType, ValueType>;

 public:
    BlackBoard() = default;
    ~BlackBoard() = default;
    BlackBoard(BlackBoard&& rhs)
    {
        operator=(std::move(rhs));
    }
    BlackBoard& operator=(BlackBoard&& rhs)
    {
        if (this != &rhs)
        {
            _handleContainer = std::move(rhs._handleContainer);
        }
        return *this;
    }

 public:
    ResourceHandle getHandle(std::string_view name) const
    {
        ContainerType::const_iterator it = _handleContainer.find(name);
        if (it != _handleContainer.cend())
        {
            return it->second;
        }
        return InvalidFrameGraphResource;
    }
    ResourceHandle& operator[](std::string_view name)
    {
        auto [iter, _] =
            _handleContainer.insert_or_assign(name, InvalidFrameGraphResource);
        return iter->second;
    }

    bool removeHandle(std::string_view name)
    {
        _handleContainer.erase(_handleContainer.find(name));
    }

 private:
    ContainerType _handleContainer;
};
}  // namespace FrameGraph

}  // namespace VoxFlow

#endif