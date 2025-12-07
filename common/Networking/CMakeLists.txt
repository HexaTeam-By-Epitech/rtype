# Networking Module CMake Configuration

cmake_minimum_required(VERSION 3.20)

# Find ENet library
find_package(enet CONFIG REQUIRED)

# Define the networking library
add_library(rtype_networking STATIC
    NetworkFactory.cpp
    enet/ENetAddress.cpp
    enet/ENetPacket.cpp
    enet/ENetPeer.cpp
    enet/ENetHost.cpp
)

# Set include directories
target_include_directories(rtype_networking
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
)

# Link ENet library
target_link_libraries(rtype_networking
    PUBLIC
        enet::enet
)

# Set C++ standard
target_compile_features(rtype_networking PUBLIC cxx_std_17)

# Add compiler warnings
if(MSVC)
    target_compile_options(rtype_networking PRIVATE /W4)
else()
    target_compile_options(rtype_networking PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Export the target
set_target_properties(rtype_networking PROPERTIES
    POSITION_INDEPENDENT_CODE ON
)
