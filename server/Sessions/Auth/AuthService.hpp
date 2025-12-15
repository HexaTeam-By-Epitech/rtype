/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** AuthService.hpp
*/

#pragma once

#include "server/Sessions/Auth/IAuthService.hpp"

namespace server {

    class AuthService : public IAuthService {
       public:
        AuthService() = default;
        ~AuthService() override = default;

        bool authenticate(const std::string &username, const std::string &password) override;
    };

}  // namespace server
