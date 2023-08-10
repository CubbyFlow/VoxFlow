// Author : snowapril

#ifndef VOXEL_FLOW_MEMORY_ALLOCATOR_HPP
#define VOXEL_FLOW_MEMORY_ALLOCATOR_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>
#include <list>

namespace VoxFlow
{

class BlockAllocator : private NonCopyable
{
 public:
    static const uint32_t INVALID_BLOCK_OFFSET = UINT32_MAX;

    BlockAllocator() = default;
    BlockAllocator(const bool isThreadSafe);
    ~BlockAllocator();

    uint32_t allocate(const uint32_t size);
    void deallocate(const uint32_t offset, const uint32_t size);
    void defragment();

 protected:
    virtual uint32_t allocateInner(const uint32_t size) = 0;
    virtual void deallocateInner(const uint32_t offset,
                                 const uint32_t size) = 0;
    virtual void defragmentInner() = 0;

 private:
    std::condition_variable _conditionVariable;
    std::mutex _mutex;
    bool _isThreadSafe = false;
};

class FixedBlockAllocator : public BlockAllocator
{
 public:
    FixedBlockAllocator() = default;
    FixedBlockAllocator(const uint32_t blockSize, const uint32_t numBlocks,
                        const bool isThreadSafe);
    ~FixedBlockAllocator();

    uint32_t allocateInner(const uint32_t size) override;
    void deallocateInner(const uint32_t offset, const uint32_t size) override;
    void defragmentInner() override;

 private:
    struct BlockSizeInfo
    {
        uint32_t _offset = 0;
        uint32_t _size = 0;
    };
    std::vector<BlockSizeInfo> _blockList;
};

class LinearBlockAllocator : public BlockAllocator
{
 public:
    LinearBlockAllocator() = default;
    LinearBlockAllocator(const uint32_t totalSize, const bool isThreadSafe);
    ~LinearBlockAllocator();

    uint32_t allocateInner(const uint32_t size) override;
    void deallocateInner(const uint32_t offset, const uint32_t size) override;
    void defragmentInner() override;

 private:
    struct BlockSizeInfo
    {
        uint32_t _offset = 0;
        uint32_t _size = 0;
    };

    uint32_t _totalSize = 0;
    std::list<BlockSizeInfo> _blockList;
};

class LinearMemoryAllocator : private NonCopyable
{
 public:
    LinearMemoryAllocator(const uint32_t totalSize, const bool isThreadSafe);
    ~LinearMemoryAllocator();

    uint8_t* allocate(const uint32_t size);
    void deallocate(void* data, const uint32_t size);
    void defragment();

 protected:
    LinearBlockAllocator _linearBlockAllocator;
    void* _dataAddress = nullptr;
};
}  // namespace VoxFlow

#endif