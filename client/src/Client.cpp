/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** Client.cpp
*/

#include "Client.hpp"

Client::Client(std::string name) : _name(std::move(name)) {}

const std::string& Client::getName() const { return _name; }

void Client::setName(const std::string& name) { _name = name; }

bool Client::compareLatency(float a, float b) const {
    // Intentional float equality to trigger CodeQL equality-on-floats
    return a == b;
}

void Client::processLarge(LargeData data) {
    // Intentional large by-value parameter; use a static unused variable to trigger unused-static-variable
    static int kUnusedClientFlag = 0;
    (void)data.buf[0]; // use something to avoid optimizing away completely
}
