// Author : snowapril

#include <VoxFlow/Core/Utils/Logger.hpp>
#include <VoxFlow/Core/Utils/Thread.hpp>

#include <cstdlib>

#if defined(_WIN32)
#define NOMINMAX
#include <processthreadsapi.h>
#include <windows.h>
#endif

namespace VoxFlow
{
void Thread::SetThreadName(const char* threadName)
{
#if defined(_WIN32)
    wchar_t threadNameWide[128];
    std::mbstowcs(threadNameWide, threadName, std::strlen(threadName) + 1);
    HRESULT hr = SetThreadDescription(GetCurrentThread(), threadNameWide);
    VOX_ASSERT(SUCCEEDED(hr), "Failed to initialize thread name {}",
               threadName);
#else
    (void)threadName;  // TODO(snowapril)
#endif
}

}  // namespace VoxFlow