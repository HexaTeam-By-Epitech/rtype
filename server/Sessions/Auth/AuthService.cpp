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
#include "common/Logger/Logger.hpp"

namespace server {

    // Constant-time string comparison to prevent timing attacks
    // Returns true if strings are equal, false otherwise
    static bool constantTimeCompare(const std::string &a, const std::string &b) {
        // If lengths differ, still compare to maintain constant time
        size_t length = a.length();
        if (b.length() != length) {
            // Continue comparison with padding to maintain timing
            length = std::max(a.length(), b.length());
        }

        volatile uint8_t result = 0;

        for (size_t i = 0; i < length; ++i) {
            uint8_t aChar = (i < a.length()) ? static_cast<uint8_t>(a[i]) : 0;
            uint8_t bChar = (i < b.length()) ? static_cast<uint8_t>(b[i]) : 0;
            result |= aChar ^ bChar;
        }

        // Also check length equality in constant time
        volatile uint8_t lengthDiff = 0;
        if (a.length() != b.length()) {
            lengthDiff = 1;
        }

        return (result | lengthDiff) == 0;
    }

    AuthService::AuthService() {
        loadAccounts();
    }

    AuthService::AuthService(const std::string &accountsFile) : _accountsFile(accountsFile) {
        loadAccounts();
    }

    bool AuthService::authenticate(const std::string &username, const std::string &password) {
        // Special case: guest login doesn't require account registration
        if (username == "guest" && password == "guest") {
            _authenticatedUsers.insert(username);
            return true;
        }

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

        // Verify password using constant-time comparison to prevent timing attacks
        if (!constantTimeCompare(it->second, password)) {
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
        // Prevent registering "guest" as a regular account
        if (username == "guest" || username.starts_with("Guest_")) {
            LOG_WARNING("Registration failed: username '", username, "' is reserved for guest access");
            return false;  // Guest is reserved for anonymous access
        }

        // Validation
        if (username.empty() || password.empty()) {
            LOG_WARNING("Registration failed: empty username or password");
            return false;
        }

        if (username.length() < 3 || password.length() < 4) {
            LOG_WARNING("Registration failed: username '", username, "' (", username.length(),
                        " chars) or password (", password.length(), " chars) too short");
            return false;
        }

        // Check if username already exists
        if (_accounts.find(username) != _accounts.end()) {
            LOG_WARNING("Registration failed: username '", username, "' already exists");
            return false;
        }

        // Register new user
        _accounts[username] = password;
        saveAccounts();
        return true;
    }

    void AuthService::loadAccounts() {
        std::ifstream file(_accountsFile, std::ios::binary);
        if (!file.is_open()) {
            // File doesn't exist, create empty file (no default accounts)
            // Guest login works without registration
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
