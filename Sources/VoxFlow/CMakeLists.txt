# Target name
set(target VoxFlow)
set(ROOT_DIR ${PROJECT_SOURCE_DIR})
set(PUBLIC_HDR_DIR ${PROJECT_SOURCE_DIR}/Includes)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})

# Set public headers
set(PUBLIC_HDRS
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Context.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Instance.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/LogicalDevice.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/PhysicalDevice.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Devices/Queue.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/DecisionMaker.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/Logger.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/Utils/NonCopyable.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Core/VoxFlow.hpp
)

# Set Sources
set(SRCS
    ${SRC_DIR}/Core/Devices/Context.cpp
    ${SRC_DIR}/Core/Devices/Instance.cpp
    ${SRC_DIR}/Core/Devices/LogicalDevice.cpp
    ${SRC_DIR}/Core/Devices/PhysicalDevice.cpp
    ${SRC_DIR}/Core/Devices/Queue.cpp
    ${SRC_DIR}/Core/Utils/DecisionMaker.cpp
)

# Build library
add_library(${target} ${SRCS} ${BACKWARD_ENABLE})

# Speed up builds
cotire(${target})

# Enable backward-cpp stack-trace for this build
add_backward(${target})

# Set precompiled header
set(PRECOMPILED_HEADERS
    ${PUBLIC_HDR_DIR}/Utils/pch.hpp
)
set_target_properties(${target}
    PROPERTIES
    COTIRE_CXX_PREFIX_HEADER_INIT ${PRECOMPILED_HEADERS}
)

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
    # ${glm_INCLUDE_DIR}
    # ${tinyobjloader_INCLUDE_DIR}
    # ${glfw_INCLUDE_DIR}
    PRIVATE
    ${PUBLIC_HDR_DIR}
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
)

# Compile definitions
target_compile_definitions(${target}
    PRIVATE
    RESOURCES_DIR="${RESOURCES_DIR}"
    ${DEFAULT_COMPILE_DEFINITIONS}
)

# Install
install(TARGETS  ${target}
        EXPORT   ${target}-config
        RUNTIME  DESTINATION bin
        LIBRARY  DESTINATION lib
        ARCHIVE  DESTINATION lib
        PUBLIC_HEADER DESTINATION include
)
install(EXPORT   ${target}-config
        DESTINATION share/${target}
)