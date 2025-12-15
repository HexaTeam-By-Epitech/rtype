/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** AuthService.hpp
*/

#pragma once

#include <string>
#include <unordered_map>
#include <unordered_set>
#include "server/Sessions/Auth/IAuthService.hpp"

namespace server {

    class AuthService : public IAuthService {
       public:
        AuthService() = default;
        ~AuthService() override = default;

        /**
         * @brief Authenticate a user with username and password.
         * @param username The username
         * @param password The password
         * @return bool True if authentication succeeds
         */
        bool authenticate(const std::string &username, const std::string &password) override;

        /**
         * @brief Generate an authentication token for a user.
         * @param username The username
         * @return std::string The generated token
         */
        std::string generateToken(const std::string &username);

        /**
         * @brief Validate a token.
         * @param token The token to validate
         * @return bool True if token is valid
         */
        bool validateToken(const std::string &token);

        /**
         * @brief Revoke a token.
         * @param token The token to revoke
         */
        void revokeToken(const std::string &token);

        /**
         * @brief Check if a user is authenticated.
         * @param username The username to check
         * @return bool True if user is authenticated
         */
        bool isUserAuthenticated(const std::string &username) const;

       private:
        std::unordered_set<std::string> _authenticatedUsers;         ///< Set of authenticated usernames
        std::unordered_map<std::string, std::string> _activeTokens;  ///< Map of tokens to usernames
    };

}  // namespace server
