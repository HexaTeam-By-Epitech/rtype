add_library(rtype_server_lib
    src/Server.cpp
)

target_include_directories(rtype_server_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(rtype_server_lib PUBLIC cxx_std_23)

add_executable(r-type_server
        src/main.cpp
)

target_link_libraries(r-type_server PRIVATE rtype_server_lib)

target_include_directories(r-type_server PRIVATE src include)
