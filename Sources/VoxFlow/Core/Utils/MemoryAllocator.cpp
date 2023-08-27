// Author : snowapril

#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/MemoryAllocator.hpp>

namespace VoxFlow
{

BlockAllocator::BlockAllocator(const bool isThreadSafe)
    : _isThreadSafe(isThreadSafe)
{
}

BlockAllocator::~BlockAllocator()
{
}

uint64_t BlockAllocator::allocate(const uint64_t size)
{
    std::unique_lock allocatorLock(_mutex, std::defer_lock);
    if (_isThreadSafe)
    {
        allocatorLock.lock();
    }

    const uint64_t offset = allocateInner(size);

    if (_isThreadSafe)
    {
        allocatorLock.unlock();
    }

    return offset;
}

void BlockAllocator::deallocate(const uint64_t offset, const uint64_t size)
{
    std::unique_lock allocatorLock(_mutex, std::defer_lock);
    if (_isThreadSafe)
    {
        allocatorLock.lock();
    }

    deallocateInner(offset, size);

    if (_isThreadSafe)
    {
        allocatorLock.unlock();
    }
}

void BlockAllocator::defragment()
{
    std::unique_lock allocatorLock(_mutex, std::defer_lock);
    if (_isThreadSafe)
    {
        allocatorLock.lock();
    }

    defragmentInner();

    if (_isThreadSafe)
    {
        allocatorLock.unlock();
    }
}

LinearBlockAllocator::LinearBlockAllocator(const uint64_t totalSize,
                                           const bool isThreadSafe)
    : BlockAllocator(isThreadSafe), _totalSize(totalSize)
{
    _blockList.emplace(_blockList.end(), 0, _totalSize);
} 

LinearBlockAllocator::~LinearBlockAllocator()
{
}

uint64_t LinearBlockAllocator::allocateInner(const uint64_t size)
{
    uint64_t offset = INVALID_BLOCK_OFFSET;

    for (auto iter = _blockList.begin(); iter != _blockList.end();)
    {
        BlockSizeInfo& blockInfo = *iter;
        if (blockInfo._size == size)
        {
            offset = blockInfo._offset;
            iter = _blockList.erase(iter);
        }
        else 
        {
            if (blockInfo._size > size)
            {
                offset = blockInfo._offset;
                blockInfo._offset += size;
            }
            ++iter;
        }
    }

    return offset;
}

void LinearBlockAllocator::deallocateInner(const uint64_t offset,
                                      const uint64_t size)
{
    (void)offset;
    (void)size;
}

void LinearBlockAllocator::defragmentInner()
{
    // TODO(snowapril)
}

LinearMemoryAllocator::LinearMemoryAllocator(const uint64_t totalSize,
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

uint8_t* LinearMemoryAllocator::allocate(const uint64_t size)
{
    const uint64_t offset = _linearBlockAllocator.allocate(size);
    if (offset == LinearBlockAllocator::INVALID_BLOCK_OFFSET)
    {
        return nullptr;
    }

    return static_cast<uint8_t*>(_dataAddress) + offset;
}

void LinearMemoryAllocator::deallocate(void* data, const uint64_t size)
{
    const bool isInThisAllocator =
        (static_cast<uint8_t*>(_dataAddress) <= static_cast<uint8_t*>(data)) &&
        (static_cast<uint8_t*>(_dataAddress) <
         (static_cast<uint8_t*>(_dataAddress) + size));
    if (isInThisAllocator)
    {
        VOX_ASSERT(false, "Unknown memory block was given");
    }
    else
    {
        const uint64_t offset = static_cast<uint64_t>(
            static_cast<uint8_t*>(data) - static_cast<uint8_t*>(_dataAddress));
        _linearBlockAllocator.deallocate(offset, size);
    }
}

void LinearMemoryAllocator::defragment()
{
    _linearBlockAllocator.defragment();
}
}  // namespace VoxFlow