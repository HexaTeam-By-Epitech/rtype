/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IAuthService.hpp
*/

#pragma once

#include <string>
#include <memory>

namespace server {

class IAuthService {
public:
    virtual ~IAuthService() = default;

    /**
     * @brief Authenticate a user
     * @param username Username
     * @param password Password
     * @return true if credentials are valid
     */
    virtual bool authenticate(const std::string &username, const std::string &password) = 0;
};

} // namespace server

