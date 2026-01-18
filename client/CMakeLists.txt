file(GLOB_RECURSE CLIENT_SOURCES CONFIGURE_DEPENDS
        "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)

list(FILTER CLIENT_SOURCES EXCLUDE REGEX ".*/main.cpp$")

# Use SHARED on Windows (MSVC), STATIC on Linux/macOS
if(WIN32)
    add_library(rtype_client_lib SHARED ${CLIENT_SOURCES})
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

find_package(glfw3 CONFIG REQUIRED)

target_link_libraries(rtype_client_lib PUBLIC
        raylib
        glfw
        rtype_serialization
        rtype_networking
        rtype_threading
)

add_executable(r-type_client
        main.cpp
)

target_link_libraries(r-type_client PRIVATE rtype_client_lib)

# Copy DLLs to the executable directory on Windows
if(WIN32)
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying vcpkg DLLs for r-type_client..."
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_BINARY_DIR}/vcpkg_installed/x64-windows/bin" "$<TARGET_FILE_DIR:r-type_client>"
        COMMAND ${CMAKE_COMMAND} -E echo "Copying project DLLs for r-type_client..."
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:rtype_client_lib>" "$<TARGET_FILE_DIR:r-type_client>/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:rtype_networking>" "$<TARGET_FILE_DIR:r-type_client>/"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "$<TARGET_FILE:rtype_serialization>" "$<TARGET_FILE_DIR:r-type_client>/"
        COMMAND_EXPAND_LISTS
    )
else()
    add_custom_command(TARGET r-type_client POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Creating symlink for r-type_client..."
        COMMAND ${CMAKE_COMMAND} -E remove -f "${CMAKE_SOURCE_DIR}/r-type_client"
        COMMAND ${CMAKE_COMMAND} -E create_symlink "$<TARGET_FILE:r-type_client>" "${CMAKE_SOURCE_DIR}/r-type_client"
    )
endif()
