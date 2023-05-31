// Author : snowapril

#ifndef VOXEL_FLOW_CHROME_TRACER_HPP
#define VOXEL_FLOW_CHROME_TRACER_HPP

#include <condition_variable>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string_view>
#include <thread>

namespace VoxFlow
{

class ChromeTracer
{
 public:
    ChromeTracer(const ChromeTracer&) = delete;
    ChromeTracer(ChromeTracer&&) = delete;
    ChromeTracer& operator=(const ChromeTracer&) = delete;
    ChromeTracer& operator=(ChromeTracer&&) = delete;

    static ChromeTracer& Get();

 public:
    class ScopedChromeTracing
    {
        friend ChromeTracer;

     private:
        ScopedChromeTracing(ChromeTracer* ownerTracer,
                            std::string_view&& eventName);
        ~ScopedChromeTracing();

     private:
        ChromeTracer* _ownerTracer = nullptr;
        std::string_view _eventName;
    };

 private:
    ChromeTracer() = default;

 private:
    std::mutex _mutex;
    nlohmann::json _jsonFileHandle;
};

#if defined(ENABLE_CHROME_TRACING)
#define _CONCAT_(x, y) x##y
#define CONCAT(x, y) _CONCAT(x, y)
#define SCOPED_CHROME_TRACING(eventName) \
    auto CONCAT(#eventName, __LINE__) =  \
        ChromeTracer::ScopedChromeTracing(&ChromeTracer::Get(), eventName)
#else
#define SCOPED_CHROME_TRACING(eventName)
#endif  // ENABLE_CHROME_TRACING

}  // namespace VoxFlow

#endif