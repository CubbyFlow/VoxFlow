// Author : snowapril

#include <VoxFlow/Core/Utils/MemoryAllocator.hpp>

namespace VoxFlow
{
LinearBlockAllocator::LinearBlockAllocator(const uint32_t totalSize,
                                           const bool isThreadSafe)
    : _totalSize(totalSize), _isThreadSafe(isThreadSafe)
{
}

LinearBlockAllocator::~LinearBlockAllocator()
{
}

uint32_t LinearBlockAllocator::allocate(const uint32_t size)
{
    return 0;
}

void LinearBlockAllocator::deallocate(const uint32_t offset,
                                      const uint32_t size)
{
}

void LinearBlockAllocator::defragment()
{
    // TODO(snowapril)
}

LinearMemoryAllocator::LinearMemoryAllocator(const uint32_t totalSize,
                                             const bool isThreadSafe)
    : _linearBlockAllocator(totalSize, isThreadSafe)
{
    _dataAddress = malloc(totalSize);
}

LinearMemoryAllocator::~LinearMemoryAllocator()
{
    if (_dataAddress != nullptr)
    {
        free(_dataAddress);
    }
}

void* LinearMemoryAllocator::allocate(const uint32_t size)
{
    const uint32_t offset = _linearBlockAllocator.allocate(size);
    if (offset == LinearBlockAllocator::INVALID_BLOCK_OFFSET)
    {
        return nullptr;
    }

    return _dataAddress + offset;
}

void LinearMemoryAllocator::deallocate(void* data, const uint32_t size)
{
    const bool isInThisAllocator =
        (_dataAddress <= data) && (data < (_dataAddress + size));
    if (isInThisAllocator)
    {
        VOX_ASSERT(false, "Unknown memory block was given");
    }
    else
    {
        const uint32_t offset = (data - _dataAddress);
        _linearBlockAllocator.deallocate(offset, size);
    }
}

void LinearMemoryAllocator::defragment()
{
    _linearBlockAllocator.defragment();
}
}  // namespace VoxFlow