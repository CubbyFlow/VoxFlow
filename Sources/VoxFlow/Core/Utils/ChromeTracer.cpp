// Author : snowapril

#include <VoxFlow/Core/Utils/ChromeTracer.hpp>

// #if defined(ENABLE_CHROME_TRACING)
namespace VoxFlow
{

ChromeTracer& ChromeTracer::Get()
{
    static ChromeTracer sChromeTracer;
    return sChromeTracer;
}

void ChromeTracer::beginTrace()
{
    _traceStartTime = std::chrono::system_clock::now();
}

void ChromeTracer::endTrace(const char* traceFilePath)
{
    (void)traceFilePath;
    // TODO(snowapril) : implement shutdown and flushing
}

void ChromeTracer::addTraceEvent(ChromeTraceEvent eventType,
                                 const char* eventName)
{
    (void)eventType;
    (void)eventName;
    // TODO(snowapril) : implement json logging
}

ChromeTracer::ScopedChromeTracing::ScopedChromeTracing(
    ChromeTracer* ownerTracer, const char* eventName)
    : _ownerTracer(ownerTracer), _eventName(std::move(eventName))
{
    _ownerTracer->addTraceEvent(ChromeTraceEvent::DurationBegin, _eventName);
}

ChromeTracer::ScopedChromeTracing::~ScopedChromeTracing()
{
    _ownerTracer->addTraceEvent(ChromeTraceEvent::DurationEnd, _eventName);
}
}  // namespace VoxFlow

// #endif // ENABLE_CHROME_TRACING