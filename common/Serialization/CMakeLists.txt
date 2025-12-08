cmake_minimum_required(VERSION 3.20)
project(serialization)

file(GLOB SERIALIZATION_SRC
        "capnp/*.cpp"
        "SerializerFactory.cpp"
)
file(GLOB SERIALIZATION_HEADERS
        "capnp/*.hpp"
        "ISerializer.hpp"
        "SerializerFactory.hpp"
        "GameMessages.hpp"
        "GameCodec.hpp"
)

add_library(rtype_serialization SHARED ${SERIALIZATION_SRC} ${SERIALIZATION_HEADERS})

find_package(CapnProto CONFIG REQUIRED)

target_include_directories(rtype_serialization
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}        # for public headers
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/capnp  # capnp backend is internal
)

target_link_libraries(rtype_serialization PUBLIC
        CapnProto::capnp
        CapnProto::capnp-rpc
        CapnProto::kj
        CapnProto::kj-async
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)