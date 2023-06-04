// Author : snowapril

#include <VoxFlow/Core/Devices/Instance.hpp>
#include <VoxFlow/Core/Utils/DebugUtil.hpp>
#include "../../UnitTestUtils.hpp"

TEST_CASE("Vulkan Instance Initialization")
{
    VoxFlow::Instance instance(gVulkanContext);

    CHECK_NE(instance.get(), VK_NULL_HANDLE);
    CHECK_EQ(VoxFlow::DebugUtil::NumValidationErrorDetected, 0);
}