/*
** EPITECH PROJECT, 2025
** Created by mael on 17/11/2025.
** File description:
** main.cpp
*/

#include <iostream>
#include "Server.hpp"

int main() {
    Server s("127.0.0.1");
    std::cout << "Hello World from server! host=" << s.getHost() << std::endl;
    return 0;
}
