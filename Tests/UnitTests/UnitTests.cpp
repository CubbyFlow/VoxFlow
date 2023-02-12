// Author : snowapril

#define DOCTEST_CONFIG_IMPLEMENT

#include "UnitTestUtils.hpp"
#include <spdlog/spdlog.h>

VoxFlow::Context gVulkanContext;

int main()
{
    doctest::Context context;

    spdlog::set_level(spdlog::level::debug);

    gVulkanContext.setVersion(1, 3);
    gVulkanContext.addRequiredQueue(
        "GCT",
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT, 1,
        1.0F, true);

    // Run queries, or run tests unless --no-run is specified
    return context.run();
}