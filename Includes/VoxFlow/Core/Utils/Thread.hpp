// Author : snowapril

#ifndef VOXEL_FLOW_THREAD_HPP
#define VOXEL_FLOW_THREAD_HPP

#include <condition_variable>
#include <mutex>
#include <thread>

namespace VoxFlow
{

class Thread
{
 public:
    virtual void threadConstruct() = 0;
    virtual void threadProcess() = 0;
    virtual void threadTerminate() = 0;

 protected:
    std::thread _threadHandle;
    std::condition_variable _conditionVariable;
    std::mutex _mutex;
    bool _isConstructed = false;
    bool _shouldTerminate = false;
};
}  // namespace VoxFlow

#endif