cmake_minimum_required(VERSION 3.20)

enable_testing()

find_package(GTest CONFIG REQUIRED)

# Client tests (disabled until Client.hpp is created)
# add_executable(client_tests
#     client_tests/ClientTest.cpp
# )
# 
# target_include_directories(client_tests PRIVATE ${CMAKE_SOURCE_DIR}/client/include)
# 
# target_link_libraries(client_tests PRIVATE GTest::gtest_main rtype_client_lib)
# 
# add_test(NAME client_tests COMMAND client_tests)

# ECS tests
add_executable(ecs_tests
    ecs_tests/IComponentTest.cpp
    ecs_tests/ComponentsTest.cpp
)

target_include_directories(ecs_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/common/ECS
    ${CMAKE_SOURCE_DIR}/common/ECS/Components
)

target_link_libraries(ecs_tests PRIVATE GTest::gtest_main)

add_test(NAME ecs_tests COMMAND ecs_tests)

# Server tests
add_executable(server_tests
    server_tests/ServerTest.cpp
    server_tests/ServerIntegrationTest.cpp
)

target_include_directories(server_tests PRIVATE ${CMAKE_SOURCE_DIR}/server/include)

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
)

target_include_directories(serialization_tests PRIVATE 
    ${CMAKE_SOURCE_DIR}/common/serialization
    ${CMAKE_BINARY_DIR}/common/serialization
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

