cmake_minimum_required(VERSION 3.20)
project(serialization)

file(GLOB SERIALIZATION_SRC
        "Capnp/*.cpp"
)
file(GLOB_RECURSE SERIALIZATION_HEADERS
        "Capnp/*.hpp"
)

find_package(CapnProto CONFIG REQUIRED)

# Compile Cap'n Proto schemas (modular structure)
capnp_generate_cpp(CAPNP_SRCS CAPNP_HDRS
        schemas/connection.capnp
        schemas/shared_types.capnp
        schemas/c2s_messages.capnp
        schemas/s2c_messages.capnp
)

add_library(rtype_serialization SHARED
        ${SERIALIZATION_SRC}
        ${SERIALIZATION_HEADERS}
        ${CAPNP_SRCS}
        ${CAPNP_HDRS}
)

target_include_directories(rtype_serialization
        PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}        # for public headers
        ${CMAKE_CURRENT_BINARY_DIR}        # for generated Cap'n Proto headers
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Capnp  # Capnp backend is internal
)

target_link_libraries(rtype_serialization PUBLIC
        CapnProto::capnp
        CapnProto::capnp-rpc
        CapnProto::kj
        CapnProto::kj-async
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)