#ifndef VOX_FLOW_CONTEXT_HPP
#define VOX_FLOW_CONTEXT_HPP

#include <volk/volk.h>
#include <string>
#include <vector>

namespace VoxFlow
{
struct Context
{
    explicit Context(bool useValidation = true);
    Context(const Context&) = default;
    Context(Context&&) = default;
    Context& operator=(const Context&) = default;
    Context& operator=(Context&&) = default;

    void setVersion(const uint32_t major, const uint32_t minor);

    void addInstanceLayer(const char* name, bool isOptional = false);
    void addInstanceExtension(const char* name, bool isOptional = false);
    void addDeviceExtension(const char* name, bool isOptional = false,
                            void* pFeatureStruct = nullptr,
                            uint32_t version = 0);

    void addRequiredQueue(std::string&& queueName, VkQueueFlags queueFlag,
                          uint32_t queueCount, float queuePriority,
                          bool isMainQueue);

    std::string appEngine = "VoxFlow";
    std::string appTitle = "VoxFlow";

    struct Entry
    {
        explicit Entry(const char* entryName, const bool isOptional = false,
                       void* pFeatureStruct = nullptr,
                       const uint32_t version = 0)
            : entryName(entryName),
              isOptional(isOptional),
              pFeatureStruct(pFeatureStruct),
              version(version){};

        const char* entryName{ nullptr };
        bool isOptional{ false };
        void* pFeatureStruct{ nullptr };
        uint32_t version{ 0 };
    };

    std::vector<Entry> instanceLayers{};
    std::vector<Entry> instanceExtensions{};
    std::vector<Entry> deviceExtensions{};

    struct QueueSetup
    {
        explicit QueueSetup(std::string&& queueName, const VkQueueFlags flag,
                            const uint32_t queueCount, const float priority,
                            bool isMainQueue)
            : queueName(std::move(queueName)),
              flag(flag),
              queueCount(queueCount),
              priority(priority),
              isMainQueue(isMainQueue)
        {
        }

        std::string queueName;
        VkQueueFlags flag{ 0 };
        uint32_t queueCount{ 0 };
        float priority{ 1.0f };
        bool isMainQueue{ false };
    };

    std::vector<QueueSetup> requiredQueues{};

    uint32_t majorVersion{ 1 };
    uint32_t minorVersion{ 3 };
    bool useValidationLayer{ true };
};
}  // namespace VoxFlow

#endif