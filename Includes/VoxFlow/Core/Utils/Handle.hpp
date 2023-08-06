// Author : snowapril

#ifndef VOXEL_FLOW_HANDLE_HPP
#define VOXEL_FLOW_HANDLE_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <atomic>
#include <memory>

namespace VoxFlow
{

class HandleBase
{
 public:
    using HandleID = uint64_t;
    static constexpr HandleID InvalidHandleID = UINT64_MAX;

    HandleBase() : _handleId(InvalidHandleID){};
    HandleBase(HandleBase::HandleID handleID) : _handleId(handleID){};
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

    virtual bool isValid() const
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

    Handle(std::weak_ptr<Type>&& obj) : _object(std::move(obj))
    {
        static std::atomic_uint64_t sAtomicHandleID(0ULL);
        _handleId = sAtomicHandleID.fetch_add(1ULL);
    }

    Handle(const Handle& rhs)
    {
        operator=(rhs);
    }

    Handle& operator=(const Handle& rhs)
    {
        if (this != &rhs)
        {
            _object = rhs._object;
            HandleBase::operator=(rhs);
        }
        return *this;
    }
    
    Handle(Handle&& rhs)
    {
        operator=(std::move(rhs));
    }
    
    Handle& operator=(Handle&& rhs)
    {
        if (this != &rhs)
        {
            _object = std::move(rhs._object);
            HandleBase::operator=(std::move(rhs));
        }
        return *this;
    }

    inline bool isValid() const
    {
        return _object.expired() && HandleBase::isValid();
    }

    inline Type* getObjectPtr() const
    {
        return _object.get();
    }

 private:
    std::weak_ptr<Type> _object;
};

}  // namespace VoxFlow

#endif