add_library(rtype_client_lib
    src/Client.cpp
)

target_include_directories(rtype_client_lib PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

target_compile_features(rtype_client_lib PUBLIC cxx_std_23)

add_executable(r-type_client
        src/main.cpp
)

target_link_libraries(r-type_client PRIVATE rtype_client_lib)

target_include_directories(r-type_client PRIVATE src include)
