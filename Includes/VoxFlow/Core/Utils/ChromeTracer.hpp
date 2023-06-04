// Author : snowapril

#ifndef VOXEL_FLOW_CHROME_TRACER_HPP
#define VOXEL_FLOW_CHROME_TRACER_HPP

#include <chrono>
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

    // Begin chrome tracer tracing.
    void beginTrace();

    // End chrome tracer tracing and flush traced logs to given file path and
    // open. If given filepath is nullptr, then tracer will generate file to tmp
    // directory.
    void endTrace(const char* traceFilePath = nullptr);

    enum class ChromeTraceEvent
    {
        DurationBegin = 0,
        DurationEnd = 1,
    };

    // Add chrome tracing event. Will handle given event according to type
    void addTraceEvent(ChromeTraceEvent eventType, const char* eventName);

 public:
    class ScopedChromeTracing
    {
        friend ChromeTracer;

     private:
        ScopedChromeTracing(ChromeTracer* ownerTracer, const char* eventName);
        ~ScopedChromeTracing();

     private:
        ChromeTracer* _ownerTracer = nullptr;
        const char* _eventName = nullptr;
    };

 private:
    ChromeTracer() = default;

 private:
    std::mutex _mutex;
    nlohmann::json _jsonFileHandle;
    std::chrono::system_clock::time_point _traceStartTime;
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