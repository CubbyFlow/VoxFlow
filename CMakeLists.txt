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
    add_definitions(-DVOXFLOW_DEBUG)
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

# Add definitions for third-party dependencies
add_definitions(-DGLM_FORCE_SWIZZLE)
add_definitions(-DGLM_FORCE_RADIANS)
add_definitions(-DGLM_FORCE_CTOR_INIT)
add_definitions(-DGLM_ENABLE_EXPERIMENTAL)

# Includes
include_directories(Dependencies/src)
include_directories(Dependencies/src/vulkan/include)
include_directories(Dependencies/src/vma/src)
include_directories(Dependencies/src/glfw/include)
include_directories(Dependencies/src/glm)
include_directories(Dependencies/src/assimp/include)
include_directories(Dependencies/src/etc2comp/EtcLib/Etc)
include_directories(Dependencies/src/etc2comp/EtcLib/EtcCodec)
include_directories(Dependencies/src/glslang/glslang/Include)
include_directories(Dependencies/src/taskflow)
include_directories(Dependencies/src/stb)
include_directories(Dependencies/src/cubbyflow/Includes)
include_directories(Dependencies/src/glslang)
include_directories(Dependencies/src/glslang/StandAlone)
include_directories(Dependencies/src/glslang/src)
include_directories(Dependencies/src/ImGuizmo/src)
include_directories(Dependencies/src/spdlog/include)

set(VMA_BUILD_SAMPLE                    OFF CACHE BOOL "")
set(VMA_BUILD_SAMPLE_SHADERS            OFF CACHE BOOL "")
set(VMA_STATIC_VULKAN_FUNCTIONS         OFF CACHE BOOL "")
set(VMA_DYNAMIC_VULKAN_FUNCTIONS         ON CACHE BOOL "")
set(VMA_DEBUG_INITIALIZE_ALLOCATIONS     ON CACHE BOOL "")
set(VMA_RECORDING_ENABLED                ON CACHE BOOL "")
add_subdirectory(Dependencies/src/vma)

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "")
set(GLFW_INSTALL        OFF CACHE BOOL "")
add_subdirectory(Dependencies/src/glfw)

set(ASSIMP_NO_EXPORT                         ON  CACHE BOOL "")
set(ASSIMP_BUILD_DRACO                      OFF CACHE BOOL "")
set(ASSIMP_BUILD_ASSIMP_TOOLS               OFF CACHE BOOL "")
set(ASSIMP_BUILD_TESTS                      OFF CACHE BOOL "")
set(ASSIMP_INSTALL_PDB                      OFF CACHE BOOL "")
set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT   OFF CACHE BOOL "")
set(ASSIMP_BUILD_OBJ_IMPORTER                ON  CACHE BOOL "")
set(ASSIMP_BUILD_GLTF_IMPORTER               ON  CACHE BOOL "")
add_subdirectory(Dependencies/src/assimp)

set(MESHOPT_BUILD_DEMO          OFF CACHE BOOL "")
set(MESHOPT_BUILD_TOOLS         OFF CACHE BOOL "")
set(MESHOPT_BUILD_SHARED_LIBS   OFF CACHE BOOL "")
add_subdirectory(Dependencies/src/meshoptimizer)

set(ENABLE_GLSLANG_BINARIES  OFF  CACHE BOOL "")
set(ENABLE_HLSL              OFF  CACHE BOOL "")
set(ENABLE_CTEST             OFF  CACHE BOOL "")
set(ENABLE_OPT               OFF  CACHE BOOL "")
set(SKIP_GLSLANG_INSTALL      ON  CACHE BOOL "")
add_subdirectory(Dependencies/src/glslang)

if (WIN32)
   set(VOLK_STATIC_DEFINES VK_USE_PLATFORM_WIN32_KHR)
elseif()
   # TODO
endif()
add_subdirectory(Dependencies/src/volk)

set(SPIRV_CROSS_CLI             OFF CACHE BOOL "" FORCE)
set(SPIRV_CROSS_ENABLE_TESTS    OFF CACHE BOOL "")
set(SPIRV_CROSS_ENABLE_HLSL     OFF CACHE BOOL "")
set(SPIRV_CROSS_ENABLE_MSL      OFF CACHE BOOL "")
set(SPIRV_CROSS_ENABLE_CPP       ON CACHE BOOL "")
set(SPIRV_CROSS_ENABLE_REFLECT   ON CACHE BOOL "")
set(SPIRV_CROSS_ENABLE_C_API    OFF CACHE BOOL "")
add_subdirectory(Dependencies/src/spirv-cross)

set(BUILD_TESTS     OFF CACHE BOOL "")
set(BUILD_EXAMPLES  OFF CACHE BOOL "")
set(USE_CUDA        OFF CACHE BOOL "")
set(BUILD_SONARCLOUD ON CACHE BOOL "")
add_subdirectory(Dependencies/src/cubbyflow)

add_subdirectory(Dependencies/src/etc2comp)
add_subdirectory(Dependencies/src/spdlog)
add_subdirectory(Dependencies/src/backward-cpp)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set_property(TARGET glfw          PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET assimp        PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET EtcLib        PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET EtcTool       PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET meshoptimizer PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET volk          PROPERTY FOLDER "ThirdPartyLibraries")
if(WIN32)
    set_property(TARGET UpdateAssimpLibsDebugSymbolsAndDLLs PROPERTY FOLDER "ThirdPartyLibraries")
endif()
set_property(TARGET uninstall     PROPERTY FOLDER "ThirdPartyLibraries")
set_property(TARGET glslang       PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET OGLCompiler   PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET OSDependent   PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET SPIRV         PROPERTY FOLDER "ThirdPartyLibraries/glslang")
set_property(TARGET SPVRemapper   PROPERTY FOLDER "ThirdPartyLibraries/glslang")

# Project modules
add_subdirectory(Sources/VoxFlow/Core)
add_subdirectory(Sources/VoxFlow/Editor)
add_subdirectory(Tests/UnitTests)
add_subdirectory(Tests/RenderingTests)