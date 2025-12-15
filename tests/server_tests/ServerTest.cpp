/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** ServerTest.cpp
*/

#include <gtest/gtest.h>
#include "server/Server/Server.hpp"

TEST(ServerBasicTest, DummyTest) {
    ASSERT_TRUE(true);
}

TEST(ServerBasicTest, ServerCreation) {
    // Test that we can create a server with a port
    EXPECT_NO_THROW([]() {
        Server server(4242);
    }());
}

TEST(ServerBasicTest, ServerInitialization) {
    // Test that server can be initialized
    Server server(4242);
    // Note: initialization requires networking system to be available
    // This test just verifies the constructor works
    EXPECT_NO_THROW(Server(4242, 16));
}