// Author : snowapril

#ifndef VOXEL_FLOW_HANDLE_HPP
#define VOXEL_FLOW_HANDLE_HPP

#include <cstdint>
#include <utility>

namespace VoxFlow
{

class HandleBase
{
 public:
    using HandleID = uint64_t;
    static constexpr HandleID InvalidHandleID = UINT64_MAX;

    HandleBase() : _handleId(InvalidHandleID){};
    explicit HandleBase(HandleBase::HandleID handleID) : _handleId(handleID){};
    ~HandleBase(){};
    HandleBase(const HandleBase& rhs)
    {
        operator=(rhs);
    }
    HandleBase& operator=(const HandleBase& rhs)
    {
        if (this != &rhs)
        {
            _handleId = rhs._handleId;
        }
        return *this;
    }
    HandleBase(HandleBase&& rhs)
    {
        operator=(std::move(rhs));
    }
    HandleBase& operator=(HandleBase&& rhs)
    {
        if (this != &rhs)
        {
            _handleId = rhs._handleId;
            rhs._handleId = InvalidHandleID;
        }
        return *this;
    }

    inline bool isValid() const
    {
        return _handleId != InvalidHandleID;
    }

    inline HandleID getHandleID() const
    {
        return _handleId;
    }

 private:
    HandleID _handleId;
};

template <typename Type>
class Handle : public HandleBase
{
 public:
    Handle() : HandleBase(HandleBase::InvalidHandleID)
    {
    }
    explicit Handle(HandleID handleID) : HandleBase(handleID){};
    Handle(const Handle& rhs) noexcept = default;
    Handle& operator=(const Handle& rhs) noexcept = default;
    Handle(Handle&& rhs) noexcept = default;
    Handle& operator=(Handle&& rhs) noexcept = default;
};

}  // namespace VoxFlow

#endif