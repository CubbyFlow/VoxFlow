// Author : snowapril

#ifndef VOXEL_FLOW_MEMORY_ALLOCATOR_HPP
#define VOXEL_FLOW_MEMORY_ALLOCATOR_HPP

#include <VoxFlow/Core/Utils/NonCopyable.hpp>
#include <condition_variable>
#include <mutex>
#include <thread>
#include <vector>

namespace VoxFlow
{

class LinearBlockAllocator : private NonCopyable
{
 public:
    static const uint32_t INVALID_BLOCK_OFFSET = UINT32_MAX;

    LinearBlockAllocator(const uint32_t totalSize, const bool isThreadSafe);
    ~LinearBlockAllocator();

    uint32_t allocate(const uint32_t size);
    void deallocate(const uint32_t offset, const uint32_t size);
    void defragment();

 private:
    struct BlockSizeInfo
    {
        uint32_t _offset = 0;
        uint32_t _size = 0;
    };

    uint32_t _totalSize = 0;
    std::vector<BlockSizeInfo> _blocks;

    std::thread _threadHandle;
    std::condition_variable _conditionVariable;
    std::mutex _mutex;
    bool _isThreadSafe = false;
};

class LinearMemoryAllocator : private NonCopyable
{
 public:
    LinearMemoryAllocator(const uint32_t totalSize, const bool isThreadSafe);
    ~LinearMemoryAllocator();

    void* allocate(const uint32_t size);
    void deallocate(void* data, const uint32_t size);
    void defragment();

 protected:
    LinearBlockAllocator _linearBlockAllocator;
    void* _dataAddress = nullptr;
};
}  // namespace VoxFlow

#endif