cmake_minimum_required(VERSION 3.20)

# Detect vcpkg toolchain if the user didn't pass one (allows per-developer triplet)
if (NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    if (DEFINED ENV{VCPKG_ROOT})
        set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    elseif (EXISTS "$ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake")
        set(CMAKE_TOOLCHAIN_FILE "$ENV{HOME}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
    endif ()
endif ()

project(rtype LANGUAGES CXX)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Global settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

# Enable testing so ctest discovers tests added in subdirectories
enable_testing()

# Options to select what to build
option(BUILD_SERVER "Build R-Type server" ON)
option(BUILD_CLIENT "Build R-Type client" ON)

# Add subdirectories only if needed
if (BUILD_SERVER)
    add_subdirectory(server)
endif ()

if (BUILD_CLIENT)
    add_subdirectory(client)
endif ()

# Add tests directory (will use find_package(GTest) inside)
add_subdirectory(tests)
