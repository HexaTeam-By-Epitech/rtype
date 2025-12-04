cmake_minimum_required(VERSION 3.20)

project(rtype LANGUAGES CXX)

find_package(raylib REQUIRED)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_compile_options(-Wall -Wextra -Wpedantic)

enable_testing()

option(BUILD_SERVER "Build R-Type server" ON)
option(BUILD_CLIENT "Build R-Type client" ON)

if (BUILD_SERVER)
    add_subdirectory(server)
endif ()

if (BUILD_CLIENT)
    add_subdirectory(client)
endif ()

add_subdirectory(common/serialization)

add_subdirectory(tests)