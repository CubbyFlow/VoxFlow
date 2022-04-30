// Author : snowapril

#include "../../UnitTestUtils.hpp"
#include <VoxFlow/Core/Devices/Instance.hpp>

TEST_CASE("Vulkan Instance Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);

    CHECK_NE(instance.get(), VK_NULL_HANDLE);
}