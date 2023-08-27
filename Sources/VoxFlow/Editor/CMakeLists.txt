# Target name
set(target VoxFlowEditor)
set(ROOT_DIR ${PROJECT_SOURCE_DIR})
set(LIBRARY_DIR ${PROJECT_SOURCE_DIR}/Dependencies/src)
set(PUBLIC_HDR_DIR ${PROJECT_SOURCE_DIR}/Includes)
set(SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../)

# Set public headers
set(PUBLIC_HDRS
    ${PUBLIC_HDR_DIR}/VoxFlow/Editor/RenderPass/PostProcessPass.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Editor/RenderPass/SceneObjectPass.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Editor/VoxEditor.hpp
    ${PUBLIC_HDR_DIR}/VoxFlow/Editor/VoxFlow.hpp
)

# Set Sources
set(SRCS
    ${SRC_DIR}/Editor/RenderPass/PostProcessPass.cpp
    ${SRC_DIR}/Editor/RenderPass/SceneObjectPass.cpp
    ${SRC_DIR}/Editor/VoxEditor.cpp
    ${SRC_DIR}/Editor/VoxFlow.cpp
    ${SRC_DIR}/Editor/WinMain.cpp
)

# Build library
add_executable(${target} ${SRCS} ${BACKWARD_ENABLE})

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
    ${LIBRARY_DIR}/cxxopts/include
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
    VoxFlowCore
    PRIVATE
    glfw
    volk
    glslang
    SPIRV
    assimp
)

# Compile definitions
target_compile_definitions(${target}
    PRIVATE
    RESOURCES_DIR="${RESOURCES_DIR}"
    ${DEFAULT_COMPILE_DEFINITIONS}
    CUBBYFLOW_USE_CUDA
)