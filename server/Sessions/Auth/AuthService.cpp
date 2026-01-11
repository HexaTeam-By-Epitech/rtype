/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** AuthService.cpp
*/

#include "server/Sessions/Auth/AuthService.hpp"
#include <algorithm>
#include <fstream>
#include <random>
#include <sstream>

namespace server {

    AuthService::AuthService() {
        loadAccounts();
    }

    bool AuthService::authenticate(const std::string &username, const std::string &password) {
        // Basic validation
        if (username.empty() || password.empty()) {
            return false;
        }

        // Minimum length requirements
        if (username.length() < 3 || password.length() < 4) {
            return false;
        }

        // Check against stored accounts
        auto it = _accounts.find(username);
        if (it == _accounts.end()) {
            return false;  // Account doesn't exist
        }

        // Verify password (in production, use hashed passwords)
        if (it->second != password) {
            return false;  // Wrong password
        }

        // Store authenticated session
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
        return _activeTokens.contains(token);
    }

    void AuthService::revokeToken(const std::string &token) {
        auto it = _activeTokens.find(token);
        if (it != _activeTokens.end()) {
            _activeTokens.erase(it);
        }
    }

    bool AuthService::isUserAuthenticated(const std::string &username) const {
        return _authenticatedUsers.contains(username);
    }

    bool AuthService::registerUser(const std::string &username, const std::string &password) {
        // Validation
        if (username.empty() || password.empty()) {
            return false;
        }

        if (username.length() < 3 || password.length() < 4) {
            return false;
        }

        // Check if user already exists
        if (_accounts.find(username) != _accounts.end()) {
            return false;  // Username already taken
        }

        // Register new account (in production, hash the password)
        _accounts[username] = password;
        saveAccounts();

        return true;
    }

    void AuthService::loadAccounts() {
        std::ifstream file(_accountsFile, std::ios::binary);
        if (!file.is_open()) {
            // File doesn't exist, create default accounts
            _accounts["demo"] = "1234";
            _accounts["guest"] = "guest";
            _accounts["admin"] = "admin123";
            saveAccounts();
            return;
        }

        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(':');
            if (pos != std::string::npos) {
                std::string username = line.substr(0, pos);
                std::string password = line.substr(pos + 1);
                _accounts[username] = password;
            }
        }
        file.close();
    }

    void AuthService::saveAccounts() {
        std::ofstream file(_accountsFile, std::ios::binary | std::ios::trunc);
        if (!file.is_open()) {
            return;
        }

        for (const auto &[username, password] : _accounts) {
            file << username << ":" << password << "\n";
        }
        file.close();
    }

}  // namespace server
