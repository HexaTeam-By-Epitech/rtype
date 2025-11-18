/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** Server.hpp
*/

#ifndef RTYPE_SERVER_SERVER_HPP
#define RTYPE_SERVER_SERVER_HPP

#include <string>

class Server {
public:
    Server() = default;
    explicit Server(std::string host);

    std::string getHost() const;
    void setHost(const std::string& host);

private:
    std::string _host;
};

#endif // RTYPE_SERVER_SERVER_HPP