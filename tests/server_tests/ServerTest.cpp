/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** ServerTest.cpp
*/

#include <gtest/gtest.h>
#include "Server.hpp"

TEST(ServerTest, HostGetSet) {
    Server s("localhost");
    EXPECT_EQ(s.getHost(), "localhost");
    s.setHost("example.com");
    EXPECT_EQ(s.getHost(), "example.com");
}