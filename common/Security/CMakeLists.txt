cmake_minimum_required(VERSION 3.20)

project(common_security LANGUAGES CXX)

find_package(unofficial-argon2 REQUIRED)

add_library(common_security STATIC
    Argon2PasswordHasher.cpp
    Argon2PasswordHasher.hpp
    IPasswordHasher.hpp
)

target_include_directories(common_security PUBLIC
    ${CMAKE_SOURCE_DIR}
)

target_link_libraries(common_security PUBLIC
    unofficial::argon2::libargon2
)

set_target_properties(common_security PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED ON
    CXX_EXTENSIONS OFF
)
