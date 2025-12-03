file(GLOB_RECURSE SERVER_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

list(FILTER SERVER_SOURCES EXCLUDE REGEX ".*/src/main.cpp$")

add_library(rtype_server_lib ${SERVER_SOURCES})

target_include_directories(rtype_server_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(rtype_server_lib PUBLIC cxx_std_23)

add_executable(r-type_server
        src/main.cpp
)

target_link_libraries(r-type_server PRIVATE rtype_server_lib)