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
)

# Add ECS sources from common (needed by GameLogic)
file(GLOB_RECURSE ECS_SOURCES
    "${CMAKE_SOURCE_DIR}/common/ECS/*.cpp"
)

# Librairie serveur
add_library(rtype_server_lib ${SERVER_SOURCES} ${ECS_SOURCES})

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
)

# Exécutable serveur
add_executable(r-type_server
    main.cpp
)

# Link avec la lib
target_link_libraries(r-type_server PRIVATE rtype_server_lib)

