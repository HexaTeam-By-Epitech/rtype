/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** ClientTest.cpp
*/

#include <gtest/gtest.h>
#include "Client.hpp"

TEST(ClientTest, NameGetSet) {
    Client c("alice");
    EXPECT_EQ(c.getName(), "alice");
    c.setName("bob");
    EXPECT_EQ(c.getName(), "bob");
}
