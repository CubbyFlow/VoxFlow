# CMake version
cmake_minimum_required(VERSION 3.18.6 FATAL_ERROR)
cmake_policy(SET CMP0063 NEW) # visibility
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

# Include cmake modules
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/Builds/CMake")

# Declare project
project(VoxFlow)

# Set output directories
set(DEFAULT_CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_LIBRARY_OUTPUT_DIRECTORY})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

# Set enable output of compile commands during generation
set(CMAKE_EXPORT_COMPILE_COMMANDS ON CACHE INTERNAL "")

# Compile options
include(Builds/CMake/CompileOptions.cmake)

# Build type - Release by default
message("CMake build type: " ${CMAKE_BUILD_TYPE})
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

option(BUILD_COVERAGE "Build code coverage" OFF)
if (CMAKE_BUILD_TYPE MATCHES Debug AND CMAKE_COMPILER_IS_GNUCXX AND BUILD_COVERAGE)
    include(CodeCoverage)
    setup_target_for_coverage(${PROJECT_NAME}_coverage UnitTests coverage)
endif()

# Overrides
set(CMAKE_MACOSX_RPATH ON)

# Set resource directory
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

if(WIN32)
	set(PYTHON_EXECUTABLE "python")
else()
	set(PYTHON_EXECUTABLE "python3")
endif()

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")

set(EASY_PROFILER_NO_GUI ON CACHE BOOL "")
set(EASY_PROFILER_NO_SAMPLES ON CACHE BOOL "")

set(ASSIMP_NO_EXPORT ON CACHE BOOL "")
set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "")
set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "")
set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "")

set(MESHOPT_BUILD_DEMO OFF CACHE BOOL "")
set(MESHOPT_BUILD_TOOLS OFF CACHE BOOL "")
set(MESHOPT_BUILD_SHARED_LIBS OFF CACHE BOOL "")

add_subdirectory(Dependencies/src/etc2comp)

if(WIN32)
	add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

add_subdirectory(Dependencies/src/glfw)
add_subdirectory(Dependencies/src/assimp)
add_subdirectory(Dependencies/src/meshoptimizer)

set(CUBBYFLOW_BUILD_TESTS OFF CACHE BOOL "")
set(CUBBYFLOW_BUILD_EXAMPLES OFF CACHE BOOL "")
add_subdirectory(Dependencies/src/cubbyflow)

set(ENABLE_GLSLANG_BINARIES  OFF  CACHE BOOL "")
set(ENABLE_HLSL              OFF  CACHE BOOL "")
set(ENABLE_CTEST             OFF  CACHE BOOL "")
set(ENABLE_OPT               OFF  CACHE BOOL "")
set(SKIP_GLSLANG_INSTALL     ON   CACHE BOOL "")
add_subdirectory(Dependencies/src/glslang)

# Includes
include_directories(Includes)
include_directories(Dependencies/src)
include_directories(Dependencies/src/vulkan)
include_directories(Dependencies/src/glfw/include)
include_directories(Dependencies/src/glm)
include_directories(Dependencies/src/assimp/include)
include_directories(Dependencies/src/etc2comp/EtcLib/Etc)
include_directories(Dependencies/src/etc2comp/EtcLib/EtcCodec)
include_directories(Dependencies/src/glslang/glslang/Include)
include_directories(Dependencies/src/taskflow)
include_directories(${CMAKE_CURRENT_BINARY_DIR}/Dependencies/src/assimp/include)
include_directories(Dependencies/src/stb)
include_directories(Dependencies/src/cubbyflow/Includes)
include_directories(Dependencies/src/glslang)
include_directories(Dependencies/src/glslang/StandAlone)
include_directories(Dependencies/src/glslang/src)
include_directories(Dependencies/src/ImGuizmo/src)

# Project modules
add_subdirectory(Sources)
add_subdirectory(Tests/UnitTests)