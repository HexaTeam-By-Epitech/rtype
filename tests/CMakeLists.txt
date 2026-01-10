cmake_minimum_required(VERSION 3.20)

enable_testing()

find_package(GTest CONFIG REQUIRED)

# Client tests - GameLoop diagonal movement normalization and client-side prediction
add_executable(client_tests
    client_tests/ClientTest.cpp
    client_tests/GameLoopTest.cpp
)

target_include_directories(client_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/client
    ${CMAKE_SOURCE_DIR}/client/Core/GameLoop
    ${CMAKE_SOURCE_DIR}/client/Rendering
    ${CMAKE_SOURCE_DIR}/client/Graphics
    ${CMAKE_SOURCE_DIR}/common
    ${CMAKE_BINARY_DIR}/common/Serialization
)

target_link_libraries(client_tests PRIVATE 
    GTest::gtest_main
    rtype_client_lib
)

add_test(NAME client_tests COMMAND client_tests)

# ECS tests
add_executable(ecs_tests
        ecs_tests/IComponentTest.cpp
        ecs_tests/ComponentsTest.cpp
        ecs_tests/RegistryTest.cpp
        ecs_tests/SystemsTest.cpp
        ../common/ECS/Registry.cpp
        ../common/ECS/Systems/MovementSystem/MovementSystem.cpp
        ../common/ECS/Systems/HealthSystem/HealthSystem.cpp
        ../common/ECS/Systems/WeaponSystem/WeaponSystem.cpp
        ../common/ECS/Systems/ProjectileSystem/ProjectileSystem.cpp
        ../common/ECS/Systems/BoundarySystem/BoundarySystem.cpp
)

target_include_directories(ecs_tests PRIVATE
        ${CMAKE_SOURCE_DIR}/common/ECS
        ${CMAKE_SOURCE_DIR}/common/ECS/Components
        ${CMAKE_SOURCE_DIR}/common/ECS/Systems
)

target_link_libraries(ecs_tests PRIVATE GTest::gtest_main)

add_test(NAME ecs_tests COMMAND ecs_tests)

# Server tests
add_executable(server_tests
    server_tests/ServerTest.cpp
    server_tests/ServerExtendedTest.cpp
    server_tests/ServerIntegrationTest.cpp
    server_tests/RoomTest.cpp
    server_tests/RoomManagerExtendedTest.cpp
    server_tests/SessionTest.cpp
    server_tests/MatchmakingTest.cpp
    server_tests/CoreComponentsTest.cpp
    server_tests/GameLogicExtendedTest.cpp
)

target_include_directories(server_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/server/include
    ${CMAKE_SOURCE_DIR}/server
    ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(server_tests PRIVATE GTest::gtest_main rtype_server_lib)

add_test(NAME server_tests COMMAND server_tests)

# Networking tests
add_executable(networking_tests
    networking_tests/NetworkingTest.cpp
)

target_include_directories(networking_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/common/Networking
)

target_link_libraries(networking_tests PRIVATE 
    GTest::gtest_main 
    rtype_networking
)

add_test(NAME networking_tests COMMAND networking_tests)

# Serialization tests
add_executable(serialization_tests
    serialization_tests/SerializationTest.cpp
    serialization_tests/GameStateTest.cpp
    serialization_tests/GameStartTest.cpp
    serialization_tests/PlayerInputTest.cpp
)

target_include_directories(serialization_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/common/Serialization
    ${CMAKE_BINARY_DIR}/common/Serialization
)

target_link_libraries(serialization_tests PRIVATE 
    GTest::gtest_main 
    rtype_serialization
)

add_test(NAME serialization_tests COMMAND serialization_tests)

# Threading tests
add_executable(threading_tests
    threading_tests/ThreadSafeQueueTest.cpp
)

target_include_directories(threading_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/common/Threading
)

target_link_libraries(threading_tests PRIVATE 
    GTest::gtest_main 
    rtype_threading
)

add_test(NAME threading_tests COMMAND threading_tests)

# Scripting tests
add_executable(scripting_tests
    scripting_tests/LuaEngineTest.cpp
    scripting_tests/LuaSystemAdapterTest.cpp
)

target_include_directories(scripting_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/server
    ${CMAKE_SOURCE_DIR}/server/Scripting
    ${CMAKE_SOURCE_DIR}/common
)

target_link_libraries(scripting_tests PRIVATE 
    GTest::gtest_main
    rtype_server_lib
)

add_test(NAME scripting_tests COMMAND scripting_tests)

# On Windows, copy required DLLs next to test executables
if(WIN32)
    # Function to copy vcpkg DLLs for a test target
    function(copy_test_dlls target_name)
        # Copy vcpkg DLLs (enet, capnproto, etc.)
        add_custom_command(TARGET ${target_name} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Copying DLLs for ${target_name}..."
            COMMAND ${CMAKE_COMMAND} -E true
            COMMAND_EXPAND_LISTS
        )
        
        # Copy runtime DLLs if available
        set_target_properties(${target_name} PROPERTIES
            VS_DEBUGGER_ENVIRONMENT "PATH=$<TARGET_FILE_DIR:${target_name}>;%PATH%"
        )
    endfunction()

    # Copy DLLs for each test that needs them
    copy_test_dlls(server_tests)
    copy_test_dlls(networking_tests)
    copy_test_dlls(serialization_tests)
    copy_test_dlls(scripting_tests)
endif()

