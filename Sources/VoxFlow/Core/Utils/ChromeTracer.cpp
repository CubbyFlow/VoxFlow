// Author : snowapril

#include <VoxFlow/Core/Utils/ChromeTracer.hpp>

#if defined(ENABLE_CHROME_TRACING)
#include <fstream>

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
    _hasBegun = true;
}

void ChromeTracer::endTrace(const char* traceFilePath)
{
    _hasBegun = false;

    std::vector<EventDescriptor> tmpEventDescriptors;
    {
        std::lock_guard<std::mutex> scopeLock(_mutex);
        _eventDescriptors.swap(tmpEventDescriptors);
    }

    nlohmann::json events = nlohmann::json::array();
    for (const EventDescriptor& desc : tmpEventDescriptors)
    {
        const char* eventTypeString = nullptr;
        auto timePoint =
            std::chrono::time_point_cast<std::chrono::microseconds>(
                desc._timeStamp);
        auto timeStamp = std::chrono::duration_cast<std::chrono::microseconds>(
                             timePoint.time_since_epoch())
                             .count();

        switch (desc._eventType)
        {
            case EventType::DurationBegin:
                eventTypeString = "B";
                break;
            case EventType::DurationEnd:
                eventTypeString = "E";
                break;
            default:
                continue;
        }

        // HACK(snowapril) : there is no way to get integer value from
        // std::thread::id
        std::ostringstream oss;
        oss << desc._threadId;
        uint32_t threadId = std::stoi(oss.str());

        events.push_back(nlohmann::json({ { "name", desc._name },
                                          { "ph", eventTypeString },
                                          { "ts", timeStamp },
                                          { "tid", threadId } }));
    }

    _json["traceEvents"] = events;

    std::ofstream traceFile(traceFilePath);
    if (traceFile.is_open())
    {
        traceFile << _json;
        traceFile.flush();
        traceFile.close();
    }

    _json.clear();
}

ChromeTracer::ScopedChromeTracing ChromeTracer::createScopedTracingHandle(
    const char* eventName)
{
    return ChromeTracer::ScopedChromeTracing(this, eventName);
}

void ChromeTracer::addTraceEvent(EventType eventType, const char* eventName)
{
    if (_hasBegun)
    {
        std::lock_guard<std::mutex> scopeLock(_mutex);
        _eventDescriptors.emplace_back(eventName, eventType,
                                       std::chrono::system_clock::now(),
                                       std::this_thread::get_id());
    }
}

ChromeTracer::ScopedChromeTracing::ScopedChromeTracing(
    ChromeTracer* ownerTracer, const char* eventName)
    : _ownerTracer(ownerTracer), _eventName(std::move(eventName))
{
    _ownerTracer->addTraceEvent(EventType::DurationBegin, _eventName);
}

ChromeTracer::ScopedChromeTracing::~ScopedChromeTracing()
{
    _ownerTracer->addTraceEvent(EventType::DurationEnd, _eventName);
}
}  // namespace VoxFlow

#endif  // ENABLE_CHROME_TRACING