#ifndef VOX_FLOW_CONTEXT_HPP
#define VOX_FLOW_CONTEXT_HPP

#include <VoxFlow/Core/Utils/pch.hpp>
#include <string>
#include <vector>

namespace VoxFlow
{
struct Context
{
    explicit Context(bool useValidation = true);

    void setVersion(uint32_t major, uint32_t minor);

    void addInstanceLayer(const char* name, bool isOptional = false);
    void addInstanceExtension(const char* name, bool isOptional = false);
    void addDeviceExtension(const char* name, bool isOptional = false,
                            void* pFeatureStruct = nullptr,
                            uint32_t version = 0);

    void addRequiredQueue(VkQueueFlags queueFlag, uint32_t queueCount,
                          float queuePriority);

    std::string appEngine   = "VoxFlow";
    std::string appTitle    = "VoxFlow";

    struct Entry
    {
        explicit Entry(const char* entryName, bool isOptional = false,
              void* pFeatureStruct = nullptr, uint32_t version = 0)
            : entryName(entryName),
              isOptional(isOptional),
              pFeatureStruct(pFeatureStruct),
              version(version){};

        const char* entryName   { nullptr };
        bool isOptional         { false };
        void* pFeatureStruct    { false };
        uint32_t version        { 0 };
    };

    std::vector<Entry> instanceLayers;
    std::vector<Entry> instanceExtensions;
    std::vector<Entry> deviceExtensions;

    struct QueueSetup
    {
        explicit QueueSetup(VkQueueFlags flag, uint32_t queueCount,
                            float priority)
            : flag(flag), queueCount(queueCount), priority(priority){}

        VkQueueFlags flag   { 0 };
        uint32_t queueCount { 0 };
        float priority      { 1.0f };
    };

    std::vector<QueueSetup> requiredQueues;

    uint32_t majorVersion       { 1 };
    uint32_t minorVersion       { 3 };
    bool     useValidationLayer { true };
};
}

#endif