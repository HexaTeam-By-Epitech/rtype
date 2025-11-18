/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** Client.cpp
*/

#include "Client.hpp"

Client::Client(std::string name) : _name(std::move(name)) {}

std::string Client::getName() const { return _name; }

void Client::setName(const std::string& name) { _name = name; }