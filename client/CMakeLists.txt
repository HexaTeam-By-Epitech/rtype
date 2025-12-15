file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

list(FILTER CLIENT_SOURCES EXCLUDE REGEX ".*/main.cpp$")

# Use SHARED on MSVC (MSVC can export symbols properly)
# Use STATIC on MinGW (avoids symbol duplication with static capnproto)
if(WIN32 AND MSVC)
    add_library(rtype_client_lib SHARED ${CLIENT_SOURCES})
    # Enable automatic DLL export on Windows
    set_target_properties(rtype_client_lib PROPERTIES
        WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
else()
    add_library(rtype_client_lib ${CLIENT_SOURCES})
endif()

target_include_directories(rtype_client_lib PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/Graphics
)

target_compile_features(rtype_client_lib PUBLIC cxx_std_23)
target_link_libraries(rtype_client_lib PUBLIC
        raylib
        rtype_serialization
        rtype_networking
        rtype_threading
)

add_executable(r-type_client
        main.cpp
)

target_link_libraries(r-type_client PRIVATE rtype_client_lib)