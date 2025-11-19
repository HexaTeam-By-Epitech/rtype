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

    const std::string& getName() const;
    void setName(const std::string& name);

    // Intentionally added to trigger CodeQL findings for demo purposes:
    // FIXME: validate name length and characters
    bool compareLatency(float a, float b) const; // will use '==' in impl (equality-on-floats)

    // A deliberately large parameter (168 bytes) passed by value to trigger large-parameter warnings
    struct LargeData { char buf[168]; };
    void processLarge(LargeData data); // passed by value on purpose

private:
    std::string _name;
};

// This is a commented-out code example (should be detected as commented-out code by CodeQL):
// int temp = 0; // int commentedOut = 1;

#endif // RTYPE_CLIENT_CLIENT_HPP