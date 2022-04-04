// Author : snowapril

#include <VoxFlow/Core/Devices/Context.hpp>

namespace VoxFlow
{
Context::Context(bool useValidation /* true */)
    : useValidationLayer(useValidation)
{
    // Do nothing
}

void Context::setVersion(uint32_t major, uint32_t minor)
{
    majorVersion = major;
    minorVersion = minor;
}

void Context::addInstanceLayer(const char* name, bool isOptional /* false */)
{
    instanceLayers.emplace_back(name, isOptional);
}

void Context::addInstanceExtension(const char* name,
                                   bool isOptional /* false */)
{
    instanceExtensions.emplace_back(name, isOptional);
}

void Context::addDeviceExtension(const char* name, bool isOptional /* false */,
                                 void* pFeatureStruct /* nullptr */,
                                 uint32_t version /* 0 */)
{
    deviceExtensions.emplace_back(name, isOptional, pFeatureStruct, version);
}
}  // namespace VoxFlow