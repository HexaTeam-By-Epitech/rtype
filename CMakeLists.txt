cmake_minimum_required(VERSION 3.20)

project(rtype LANGUAGES CXX)

# Global settings
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

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
