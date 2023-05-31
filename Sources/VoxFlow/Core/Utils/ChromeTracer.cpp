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

ChromeTracer::ScopedChromeTracing::ScopedChromeTracing(
    ChromeTracer* ownerTracer, std::string_view&& eventName)
    : _ownerTracer(ownerTracer), _eventName(std::move(eventName))
{
}

ChromeTracer::ScopedChromeTracing::~ScopedChromeTracing()
{
    // TODO(snowapril) : add duration event to json object
}
}  // namespace VoxFlow

// #endif // ENABLE_CHROME_TRACING