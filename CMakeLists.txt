cmake_minimum_required(VERSION 3.16)
project(vivid)

set(CMAKE_CXX_STANDARD 14)

find_package(OpenGL REQUIRED)

# Dependencies
add_subdirectory(deps)
include_directories(deps/glm)
include_directories(deps/eigen-3.4.0)

add_subdirectory(src)

option(BUILD_VIVID_SAMPLES "Build vivid samples" OFF)
if (BUILD_VIVID_SAMPLES)
    message(STATUS "Building vivid samples")
    add_subdirectory(samples)
endif ()
