# Target name
set(target VoxFlowCore)
set(ROOT_DIR ${PROJECT_SOURCE_DIR})
set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/Dependencies/src)
set(PUBLIC_HDR_DIR ${PROJECT_SOURCE_DIR}/Includes)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)

# Set public headers
set(PUBLIC_HDRS
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Context.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Instance.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/LogicalDevice.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/PhysicalDevice.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Queue.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/SwapChain.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Commands/CommandBuffer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Commands/CommandPool.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Descriptors/DescriptorSet.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Descriptors/DescriptorPool.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Descriptors/DescriptorSetLayout.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/PipelineLayout.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/GlslangUtil.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/BasePipeline.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/GraphicsPipeline.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/ComputePipeline.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/ShaderModule.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/ShaderLayoutBinding.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/Pipelines/ShaderResourceCollector.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/FrameBuffer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/RenderPass.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Graphics/RenderPass/RenderPassCollector.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/Texture.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/Buffer.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Resources/RenderResourceMemoryPool.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/DebugUtil.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/HashUtil.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/DecisionMaker.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/Logger.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/NonCopyable.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/FenceObject.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/FenceObject-Impl.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/RendererCommon.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/RenderDevice.hpp
)

# Set Sources
set(SRCS
    ${SRC_DIR}/Core/Devices/Context.cpp
    ${SRC_DIR}/Core/Devices/Instance.cpp
    ${SRC_DIR}/Core/Devices/LogicalDevice.cpp
    ${SRC_DIR}/Core/Devices/PhysicalDevice.cpp
    ${SRC_DIR}/Core/Devices/Queue.cpp
    ${SRC_DIR}/Core/Devices/SwapChain.cpp
    ${SRC_DIR}/Core/Graphics/Commands/CommandBuffer.cpp
    ${SRC_DIR}/Core/Graphics/Commands/CommandPool.cpp
    ${SRC_DIR}/Core/Graphics/Descriptors/DescriptorSet.cpp
    ${SRC_DIR}/Core/Graphics/Descriptors/DescriptorPool.cpp
    ${SRC_DIR}/Core/Graphics/Descriptors/DescriptorSetLayout.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/PipelineLayout.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/GlslangUtil.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/BasePipeline.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/GraphicsPipeline.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/ComputePipeline.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/ShaderModule.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/ShaderResourceCollector.cpp
    ${SRC_DIR}/Core/Graphics/RenderPass/FrameBuffer.cpp
    ${SRC_DIR}/Core/Graphics/RenderPass/RenderPass.cpp
    ${SRC_DIR}/Core/Graphics/RenderPass/RenderPassCollector.cpp
    ${SRC_DIR}/Core/Resources/Texture.cpp
    ${SRC_DIR}/Core/Resources/Buffer.cpp
    ${SRC_DIR}/Core/Resources/RenderResourceMemoryPool.cpp
    ${SRC_DIR}/Core/Utils/DecisionMaker.cpp
    ${SRC_DIR}/Core/Utils/DebugUtil.cpp
    ${SRC_DIR}/Core/Utils/RendererCommon.cpp
    ${SRC_DIR}/Core/RenderDevice.cpp
)

# Build library
add_library(${target} ${SRCS} ${BACKWARD_ENABLE})

# Enable backward-cpp stack-trace for this build
add_backward(${target})

# Project options
set_target_properties(${target}
    PROPERTIES
    ${DEFAULT_PROJECT_OPTIONS}
    PUBLIC_HEADER "${PUBLIC_HDRS}"
)

#Include directories
target_include_directories(${target}
    PUBLIC
    $<BUILD_INTERFACE:${PUBLIC_HDR_DIR}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/include>
    PRIVATE
    ${PUBLIC_HDR_DIR}
    ${LIBRARY_DIR}/glfw/include
    ${LIBRARY_DIR}/volk
    ${LIBRARY_DIR}/glslang
    ${LIBRARY_DIR}/assimp/include
    ${LIBRARY_DIR}/vma/src
    ${LIBRARY_DIR}
)

# Compile options
target_compile_options(${target}
    PUBLIC
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

# Link libraries
target_link_libraries(${target}
    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}
	${DEFAULT_LIBRARIES}
    PRIVATE
    glfw
    volk
    glslang
    SPIRV
    assimp
    spirv-cross-core
    VulkanMemoryAllocator
)

# Compile definitions
target_compile_definitions(${target}
    PRIVATE
    RESOURCES_DIR="${RESOURCES_DIR}"
    ${DEFAULT_COMPILE_DEFINITIONS}
)

# Install
install(TARGETS  
        ${target} 
        RUNTIME  DESTINATION bin
        LIBRARY  DESTINATION lib
        ARCHIVE  DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)