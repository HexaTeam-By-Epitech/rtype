# Sources serveur
file(GLOB_RECURSE SERVER_SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Events/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Game/**/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Rooms/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Rooms/**/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sessions/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Sessions/**/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Network/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Server/*.cpp"
        "${CMAKE_CURRENT_SOURCE_DIR}/Scripting/*.cpp"
)

# Add ECS sources from common (needed by GameLogic)
file(GLOB_RECURSE ECS_SOURCES
        "${CMAKE_SOURCE_DIR}/common/ECS/*.cpp"
        "${CMAKE_SOURCE_DIR}/common/ECSWrapper/*.cpp"
)

# Find Lua and sol2
find_package(lua REQUIRED)
find_package(sol2 REQUIRED)

# Librairie serveur
if (WIN32)
    # On Windows (MSVC), use SHARED to properly export symbols
    add_library(rtype_server_lib SHARED ${SERVER_SOURCES} ${ECS_SOURCES})
    set_target_properties(rtype_server_lib PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS TRUE
    )
else ()
    # On Linux/macOS, default to STATIC
    add_library(rtype_server_lib ${SERVER_SOURCES} ${ECS_SOURCES})
endif ()

# Include paths pour la lib
target_include_directories(rtype_server_lib
        PUBLIC
        ${CMAKE_SOURCE_DIR}
)

target_compile_features(rtype_server_lib PUBLIC cxx_std_23)

target_link_libraries(rtype_server_lib PUBLIC
        rtype_serialization
        rtype_networking
        rtype_threading
        lua
        sol2::sol2
)

# Exécutable serveur
add_executable(r-type_server
        main.cpp
        Scripting/LuaBindings/ComponentBindingHelper.cpp
        Scripting/LuaBindings/ComponentBindingHelper.hpp
        Scripting/LuaBindings/ComponentBindings.cpp
        Scripting/LuaBindings/ComponentBindings.hpp
        Scripting/LuaBindings/EntityBindings.cpp
        Scripting/LuaBindings/EntityBindings.hpp
        Scripting/LuaBindings/WorldBindings.cpp
        Scripting/LuaBindings/WorldBindings.hpp
        Scripting/LuaBindings/ServerGameBindings.cpp
        Scripting/LuaBindings/ServerGameBindings.hpp
)

# Link avec la lib
target_link_libraries(r-type_server PRIVATE rtype_server_lib)

# Copy DLLs to the executable directory on Windows
if(WIN32)
    # Get the vcpkg_installed directory
    get_filename_component(VCPKG_INSTALLED_DIR "${CMAKE_BINARY_DIR}/vcpkg_installed" ABSOLUTE)
    
    # Copy all DLLs from vcpkg_installed bin directories
    add_custom_command(TARGET r-type_server POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying DLLs for r-type_server..."
        COMMAND ${CMAKE_COMMAND} -P "${CMAKE_SOURCE_DIR}/copy_dlls.cmake" "$<TARGET_FILE_DIR:r-type_server>" "${VCPKG_INSTALLED_DIR}" "${CMAKE_BINARY_DIR}"
    )
endif()
