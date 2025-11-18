/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** Server.cpp
*/

#include "Server.hpp"

Server::Server(std::string host) : _host(std::move(host)) {}

std::string Server::getHost() const { return _host; }

void Server::setHost(const std::string& host) { _host = host; }