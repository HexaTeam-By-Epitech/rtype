/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** AuthService.cpp
*/

#include "server/Sessions/Auth/AuthService.hpp"
#include <algorithm>
#include <random>
#include <sstream>

namespace server {

    bool AuthService::authenticate(const std::string &username, const std::string &password) {
        // Basic validation
        if (username.empty() || password.empty()) {
            return false;
        }

        // For now, allow any non-empty username/password combination
        // In production, this would check against a database
        // Minimum length requirements
        if (username.length() < 3 || password.length() < 4) {
            return false;
        }

        // Store authenticated session (in production, store in database with token)
        _authenticatedUsers.insert(username);

        return true;
    }

    std::string AuthService::generateToken(const std::string &username) {
        // Generate a simple session token (in production, use JWT or secure tokens)
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dis(0, 15);

        const char *hex = "0123456789abcdef";
        std::ostringstream token;
        token << username << "_";
        for (int i = 0; i < 32; ++i) {
            token << hex[dis(gen)];
        }

        _activeTokens[token.str()] = username;
        return token.str();
    }

    bool AuthService::validateToken(const std::string &token) {
        return _activeTokens.find(token) != _activeTokens.end();
    }

    void AuthService::revokeToken(const std::string &token) {
        auto it = _activeTokens.find(token);
        if (it != _activeTokens.end()) {
            _activeTokens.erase(it);
        }
    }

    bool AuthService::isUserAuthenticated(const std::string &username) const {
        return _authenticatedUsers.find(username) != _authenticatedUsers.end();
    }

}  // namespace server
