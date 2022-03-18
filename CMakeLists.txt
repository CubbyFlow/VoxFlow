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

# Compile options
include(Builds/CMake/CompileOptions.cmake)

# Build type - Release by default
message("CMake build type: " ${CMAKE_BUILD_TYPE})
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
elseif(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
endif()

# Overrides
set(CMAKE_MACOSX_RPATH ON)

# Set resource directory
set(RESOURCES_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Resources/")

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# Project modules
add_subdirectory(Sources)