set(target UnitTests)
set(ROOT_DIR ${PROJECT_SOURCE_DIR})
set(PUBLIC_HDR_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Includes
set(PUBLIC_HDRS 
    ${PUBLIC_HDR_DIR}/UnitTestUnits.hpp
)

# Sources
set(SRCS
    ${SRC_DIR}/Core/Devices/InstanceTests.cpp
    ${SRC_DIR}/Core/Devices/PhysicalDeviceTests.cpp
    ${SRC_DIR}/Core/Devices/LogicalDeviceTests.cpp
    ${SRC_DIR}/Core/Devices/QueueTests.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/GraphicsPipelineTests.cpp
    ${SRC_DIR}/Core/Graphics/Pipelines/ComputePipelineTests.cpp
    ${SRC_DIR}/UnitTests.cpp
)

# Build executable
add_executable(${target} ${SRCS})

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
    ${ROOT_DIR}/Dependencies/src/doctest
    ${ROOT_DIR}/Includes
)

# Compile options
target_compile_options(${target}
    PRIVATE
    ${DEFAULT_COMPILE_OPTIONS}
)

# Compile definitions
target_compile_definitions(${target}
    PRIVATE
    RESOURCES_DIR="${RESOURCES_DIR}"
    ${DEFAULT_COMPILE_DEFINITIONS}
)

# Link libraries
target_link_libraries(${target}
    PUBLIC
    ${DEFAULT_LINKER_OPTIONS}
	${DEFAULT_LIBRARIES}
    VoxFlow
)