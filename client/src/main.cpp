/*
** EPITECH PROJECT, 2025
** Created by mael on 17/11/2025.
** File description:
** main.cpp
*/

#include <iostream>
#include "Client.hpp"

int main() {
    Client c("player1");
    std::cout << "Hello World from client! name=" << c.getName() << std::endl;

    constexpr int helloWorld = 0;
    std::cout << helloWorld << std::endl;
    return 0;
}