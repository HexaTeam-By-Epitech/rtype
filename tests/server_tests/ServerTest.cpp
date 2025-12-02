/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** ServerTest.cpp
*/

#include <gtest/gtest.h>
#include "Server.hpp"

TEST(ServerTest, DummyTest) {
    ASSERT_TRUE(true);
}

TEST(ServerTest, StoresInitialHost) {
    Server server{"192.168.1.5"};
    EXPECT_EQ(server.getHost(), "192.168.1.5");
}

TEST(ServerTest, UpdatesHostValue) {
    Server server{"localhost"};
    server.setHost("10.0.0.1");
    EXPECT_EQ(server.getHost(), "10.0.0.1");
}

TEST(ServerTest, ComparesPingValues) {
    Server server;
    EXPECT_TRUE(server.isPingEqual(16.0f, 16.0f));
    EXPECT_FALSE(server.isPingEqual(16.0f, 17.0f));
}

TEST(ServerTest, HandlesPacketSafely) {
    Server server;
    Server::LargePacket pkt{};
    pkt.buf[0] = 42;
    EXPECT_NO_THROW(server.handlePacket(pkt));
}
