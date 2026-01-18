/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** AuthService.cpp
*/

#include "server/Sessions/Auth/AuthService.hpp"
#include <algorithm>
#include <chrono>
#include <cstring>
#include <fstream>
#include <nlohmann/json.hpp>
#include <random>
#include <sstream>
#include "common/Logger/Logger.hpp"
#include "common/Security/Argon2PasswordHasher.hpp"

using json = nlohmann::json;

namespace server {

    AuthService::AuthService() : _passwordHasher(std::make_unique<Argon2PasswordHasher>()) {
        loadAccounts();
    }

    AuthService::AuthService(const std::string &accountsFile)
        : _accountsFile(accountsFile), _passwordHasher(std::make_unique<Argon2PasswordHasher>()) {
        loadAccounts();
    }

    AuthService::~AuthService() {
        if (_accountsDirty) {
            saveAccounts();
        }
    }

    bool AuthService::authenticate(const std::string &username, const std::string &password) {
        // Special case: guest login doesn't require account registration
        if (username == "guest" && password == "guest") {
            _authenticatedUsers.insert(username);
            return true;
        }

        if (username.empty() || password.empty()) {
            LOG_WARNING("Authentication failed: empty credentials");
            return false;
        }

        // Minimum length requirements
        if (username.length() < 3 || password.length() < 4) {
            LOG_WARNING("Authentication failed: credentials too short (username: ", username.length(),
                        ", password: ", password.length(), " chars)");
            return false;
        }

        // Check against stored accounts
        auto it = _accounts.find(username);
        if (it == _accounts.end()) {
            LOG_WARNING("Authentication failed: account '", username, "' doesn't exist");
            return false;  // Account doesn't exist
        }

        // Verify password using password hasher
        if (!_passwordHasher->verify(password, it->second.passwordHash)) {
            LOG_WARNING("Authentication failed: incorrect password for '", username, "'");
            return false;  // Wrong password
        }

        // Update last login timestamp
        auto now = std::chrono::system_clock::now();
        uint64_t nowSeconds =
            std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        it->second.lastLogin = nowSeconds;
        _accountsDirty = true;

        // Deferred save: only save if enough time has passed
        if (nowSeconds - _lastSaveTime >= SAVE_INTERVAL_SECONDS) {
            saveAccounts();
            _lastSaveTime = nowSeconds;
            _accountsDirty = false;
        }

        // Store authenticated session
        _authenticatedUsers.insert(username);

        LOG_INFO("✓ Authentication successful for user: ", username);
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

        if (username.length() < 3) {
            LOG_WARNING("Registration failed: username '", username, "' too short (", username.length(),
                        " chars, minimum 3)");
            return false;
        }

        if (password.length() < 4) {
            LOG_WARNING("Registration failed: password too short (", password.length(), " chars, minimum 4)");
            return false;
        }

        // Check if username already exists
        if (_accounts.find(username) != _accounts.end()) {
            LOG_WARNING("Registration failed: username '", username, "' already exists");
            return false;
        }

        // Hash the password
        std::string passwordHash;
        try {
            passwordHash = _passwordHasher->hash(password);
        } catch (const std::exception &e) {
            LOG_ERROR("Registration failed: password hashing failed for '", username, "': ", e.what());
            return false;
        }

        // Create new account
        auto now = std::chrono::system_clock::now();
        uint64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();

        AccountData account;
        account.username = username;
        account.passwordHash = passwordHash;
        account.createdAt = timestamp;
        account.lastLogin = 0;  // Never logged in yet

        _accounts[username] = account;
        saveAccounts();
        _accountsDirty = false;

        LOG_INFO("✓ Registration successful for user: ", username);
        return true;
    }

    void AuthService::loadAccounts() {
        std::ifstream file(_accountsFile);
        if (!file.is_open()) {
            LOG_INFO("No accounts file found at '", _accountsFile, "', starting with empty database");
            // File doesn't exist, start with empty accounts
            // Guest login works without registration
            return;
        }

        // Check if file is empty
        file.seekg(0, std::ios::end);
        if (file.tellg() == 0) {
            LOG_INFO("Accounts file is empty, starting with empty database");
            file.close();
            return;
        }
        file.seekg(0, std::ios::beg);

        try {
            json j;
            file >> j;
            file.close();

            if (!j.contains("accounts") || !j["accounts"].is_array()) {
                LOG_WARNING("Invalid accounts file format, starting with empty database");
                return;
            }

            for (const auto &accountJson : j["accounts"]) {
                if (!accountJson.contains("username") || !accountJson.contains("passwordHash")) {
                    LOG_WARNING("Skipping invalid account entry (missing username or passwordHash)");
                    continue;
                }

                AccountData account;
                account.username = accountJson["username"].get<std::string>();
                account.passwordHash = accountJson["passwordHash"].get<std::string>();
                account.createdAt = accountJson.value("createdAt", 0ULL);
                account.lastLogin = accountJson.value("lastLogin", 0ULL);
                account.autoMatchmaking = accountJson.value("autoMatchmaking", false);

                _accounts[account.username] = account;
            }

            LOG_INFO("✓ Loaded ", _accounts.size(), " accounts from '", _accountsFile, "'");

        } catch (const json::exception &e) {
            LOG_ERROR("Failed to parse accounts file: ", e.what());
            LOG_WARNING("Starting with empty accounts database");
        }
    }

    void AuthService::saveAccounts() {
        try {
            json j;
            j["version"] = "1.0";
            j["accounts"] = json::array();

            for (const auto &[username, account] : _accounts) {
                json accountJson;
                accountJson["username"] = account.username;
                accountJson["passwordHash"] = account.passwordHash;
                accountJson["createdAt"] = account.createdAt;
                accountJson["lastLogin"] = account.lastLogin;
                accountJson["autoMatchmaking"] = account.autoMatchmaking;

                j["accounts"].push_back(accountJson);
            }

            std::ofstream file(_accountsFile);
            if (!file.is_open()) {
                LOG_ERROR("Failed to open accounts file '", _accountsFile, "' for writing");
                return;
            }

            file << j.dump(2);  // Pretty print with 2 spaces indentation
            file.close();

            LOG_INFO("✓ Saved ", _accounts.size(), " accounts to '", _accountsFile, "'");

        } catch (const json::exception &e) {
            LOG_ERROR("Failed to save accounts: ", e.what());
        }
    }

    bool AuthService::updateAutoMatchmaking(const std::string &username, bool enabled) {
        auto it = _accounts.find(username);
        if (it == _accounts.end()) {
            LOG_WARNING("Cannot update auto-matchmaking: user '", username, "' not found");
            return false;
        }

        it->second.autoMatchmaking = enabled;

        // Force immediate save for user preferences (important UX)
        saveAccounts();
        auto now = std::chrono::system_clock::now();
        _lastSaveTime = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        _accountsDirty = false;

        LOG_INFO("✓ Auto-matchmaking ", enabled ? "enabled" : "disabled", " for user '", username,
                 "' (saved)");
        return true;
    }

    bool AuthService::getAutoMatchmaking(const std::string &username) const {
        auto it = _accounts.find(username);
        if (it == _accounts.end()) {
            return false;  // Default: auto-matchmaking disabled
        }
        return it->second.autoMatchmaking;
    }

}  // namespace server
