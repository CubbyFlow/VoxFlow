// Author : snowapril

#ifndef VOXEL_FLOW_CHROME_TRACER_HPP
#define VOXEL_FLOW_CHROME_TRACER_HPP

#if defined(ENABLE_CHROME_TRACING)
#include <chrono>
#include <condition_variable>
#include <mutex>
#include <nlohmann/json.hpp>
#include <string_view>
#include <thread>
#endif // ENABLE_CHROME_TRACING

namespace VoxFlow
{

#if defined(ENABLE_CHROME_TRACING)
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

    inline bool hasBegun() const
    {
        return _hasBegun;
    }

 private:
    class ScopedChromeTracing
    {
        friend ChromeTracer;

     private:
        ScopedChromeTracing(ChromeTracer* ownerTracer, const char* eventName);

     public:
        ~ScopedChromeTracing();

     private:
        ChromeTracer* _ownerTracer = nullptr;
        const char* _eventName = nullptr;
    };

 public:
    ScopedChromeTracing createScopedTracingHandle(const char* eventName);

 private:
    enum class EventType
    {
        DurationBegin = 0,
        DurationEnd = 1,
    };

    // Add chrome tracing event. Will handle given event according to type
    void addTraceEvent(EventType eventType, const char* eventName);

 private:
    ChromeTracer() = default;

private:
    struct EventDescriptor
    {
        std::string _name;
        EventType _eventType;
        std::chrono::system_clock::time_point _timeStamp;
        std::thread::id _threadId;
    };

private:
    std::mutex _mutex;
    nlohmann::json _json;
    std::chrono::system_clock::time_point _traceStartTime;
    std::vector<EventDescriptor> _eventDescriptors;
    bool _hasBegun = false;
};

#define HAS_TRACING_BEGIN() ChromeTracer::Get().hasBegun()
#define BEGIN_CHROME_TRACING() ChromeTracer::Get().beginTrace()
#define END_CHROME_TRACING(traceFilePath) \
    ChromeTracer::Get().endTrace(traceFilePath)
#define SCOPED_CHROME_TRACING(eventName) \
    auto scopdChromeTracing##__LINE__ =   \
        ChromeTracer::Get().createScopedTracingHandle(eventName);
#else
#define HAS_TRACING_BEGIN() false
#define BEGIN_CHROME_TRACING()
#define END_CHROME_TRACING(traceFilePath)
#define SCOPED_CHROME_TRACING(eventName)
#endif  // ENABLE_CHROME_TRACING

}  // namespace VoxFlow

#endif