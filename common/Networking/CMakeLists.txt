cmake_minimum_required(VERSION 3.20)
project(networking)

file(GLOB NETWORKING_SRC
    "NetworkFactory.cpp"
    "Enet/*.cpp"
)
file(GLOB NETWORKING_HEADERS
    "NetworkFactory.hpp"
    "IAddress.hpp"
    "IHost.hpp"
    "IPacket.hpp"
    "IPeer.hpp"
    "Enet/*.hpp"
)

add_library(rtype_networking SHARED ${NETWORKING_SRC} ${NETWORKING_HEADERS})

# Enable automatic symbol export on Windows
if(WIN32)
    set_target_properties(rtype_networking PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
endif()

# Find ENet library
find_package(unofficial-enet CONFIG REQUIRED)

# Set include directories
target_include_directories(rtype_networking
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}        # for public headers
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Enet  # Enet wrapper is internal
)

# Link ENet library
target_link_libraries(rtype_networking
    PUBLIC
        unofficial::enet::enet
)

# Add Windows-specific libraries for ENet
if(WIN32)
    target_link_libraries(rtype_networking
        PUBLIC
            ws2_32
            winmm
    )
endif()

# Set C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)