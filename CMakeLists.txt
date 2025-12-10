cmake_minimum_required(VERSION 3.20)

project(rtype LANGUAGES CXX)

find_package(raylib REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Suppress warnings from external libraries
if (DEFINED VCPKG_INCLUDE_DIRS)
    foreach(dir IN LISTS VCPKG_INCLUDE_DIRS)
        include_directories(SYSTEM ${dir})
    endforeach()
else()
    file(GLOB_RECURSE VCPKG_SYSTEM_INCLUDES
        "${CMAKE_SOURCE_DIR}/vcpkg_installed/*/include")
    foreach(dir IN LISTS VCPKG_SYSTEM_INCLUDES)
        include_directories(SYSTEM ${dir})
    endforeach()
endif()

add_compile_options(-Wall -Wextra -Wpedantic)

enable_testing()

option(BUILD_SERVER "Build R-Type server" ON)
option(BUILD_CLIENT "Build R-Type client" ON)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Coverage configuration
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
    add_link_options(--coverage)
endif()

if (BUILD_SERVER)
    add_subdirectory(server)
endif ()

if (BUILD_CLIENT)
    add_subdirectory(client)
endif ()

add_subdirectory(common/Serialization)
add_subdirectory(common/Networking)
add_subdirectory(common/Threading)

add_subdirectory(tests)
