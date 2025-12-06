cmake_minimum_required(VERSION 3.25)
project(rtype_server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Sources serveur
file(GLOB_RECURSE SERVER_SOURCES
    "${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Events/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Game/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Rooms/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/Sessions/**/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Librairie serveur
add_library(rtype_server_lib ${SERVER_SOURCES})

# Include paths pour la lib
target_include_directories(rtype_server_lib
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/Events
        ${CMAKE_CURRENT_SOURCE_DIR}/Events/Core
        ${CMAKE_CURRENT_SOURCE_DIR}/Events/GameEvent
        ${CMAKE_CURRENT_SOURCE_DIR}/Events/NetworkEvent
        ${CMAKE_CURRENT_SOURCE_DIR}/Game
        ${CMAKE_CURRENT_SOURCE_DIR}/Game/Prefabs
        ${CMAKE_CURRENT_SOURCE_DIR}/Game/StateManager
        ${CMAKE_CURRENT_SOURCE_DIR}/Game/World
        ${CMAKE_CURRENT_SOURCE_DIR}/Rooms
        ${CMAKE_CURRENT_SOURCE_DIR}/Rooms/RoomManager
        ${CMAKE_CURRENT_SOURCE_DIR}/Rooms/Matchmaking
        ${CMAKE_CURRENT_SOURCE_DIR}/Sessions
        ${CMAKE_CURRENT_SOURCE_DIR}/Sessions/Session
        ${CMAKE_CURRENT_SOURCE_DIR}/Sessions/SessionManager
)

target_compile_features(rtype_server_lib PUBLIC cxx_std_23)

# Exécutable serveur
add_executable(r-type_server
    src/main.cpp
)

# Link avec la lib
target_link_libraries(r-type_server PRIVATE rtype_server_lib)

# Include pour l’exécutable
target_include_directories(r-type_server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

