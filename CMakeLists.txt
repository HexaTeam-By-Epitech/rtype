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

# Compiler-specific warning flags
if(MSVC)
    # MSVC warning flags
    add_compile_options(/W4)  # Warning level 4 (similar to -Wall -Wextra)
    add_compile_options(/permissive-)  # Standards conformance
else()
    # GCC/Clang warning flags
    add_compile_options(-Wall -Wextra -Wpedantic)
endif()

enable_testing()

option(BUILD_SERVER "Build R-Type server" ON)
option(BUILD_CLIENT "Build R-Type client" ON)
option(ENABLE_COVERAGE "Enable code coverage reporting" OFF)

# Coverage configuration
if(ENABLE_COVERAGE)
    message(STATUS "Code coverage enabled")
    if(NOT MSVC)
        # Coverage is only supported with GCC/Clang
        add_compile_options(--coverage -fprofile-arcs -ftest-coverage)
        add_link_options(--coverage)
    else()
        message(WARNING "Code coverage is not supported with MSVC. Use OpenCppCoverage instead.")
    endif()
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
