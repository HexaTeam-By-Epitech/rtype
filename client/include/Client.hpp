/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** Client.hpp
*/

#ifndef RTYPE_CLIENT_CLIENT_HPP
#define RTYPE_CLIENT_CLIENT_HPP

#include <string>

class Client {
public:
    Client() = default;
    explicit Client(std::string name);

    std::string getName() const;
    void setName(const std::string& name);

private:
    std::string _name;
};

#endif // RTYPE_CLIENT_CLIENT_HPP