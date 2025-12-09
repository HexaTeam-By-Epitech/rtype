cmake_minimum_required(VERSION 3.20)
project(threading)

# Header-only library for threading utilities
add_library(rtype_threading INTERFACE)

target_include_directories(rtype_threading
        INTERFACE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

