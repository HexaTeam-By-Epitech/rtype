/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** AuthService.hpp
*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include "common/Security/IPasswordHasher.hpp"
#include "server/Sessions/Auth/IAuthService.hpp"

namespace server {

    struct AccountData {
        std::string username;
        std::string passwordHash;  // Argon2id hash
        uint64_t createdAt;        // timestamp
        uint64_t lastLogin;        // timestamp
    };

    class AuthService : public IAuthService {
       public:
        AuthService();
        explicit AuthService(const std::string &accountsFile);
        ~AuthService() override;

        /**
         * @brief Authenticate a user with username and password.
         * @param username The username
         * @param password The password (plaintext, will be verified against hash)
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

        /**
         * @brief Register a new user account
         * @param username The username
         * @param password The password (plaintext, will be hashed)
         * @return bool True if registration succeeds
         */
        bool registerUser(const std::string &username, const std::string &password);

        /**
         * @brief Load user accounts from JSON file
         */
        void loadAccounts();

        /**
         * @brief Save user accounts to JSON file
         */
        void saveAccounts();

       private:
        std::string _accountsFile = "accounts.json";                 ///< JSON file to store accounts
        std::unique_ptr<IPasswordHasher> _passwordHasher;            ///< Password hashing implementation
        std::unordered_set<std::string> _authenticatedUsers;         ///< Set of authenticated usernames
        std::unordered_map<std::string, std::string> _activeTokens;  ///< Map of tokens to usernames
        std::unordered_map<std::string, AccountData> _accounts;      ///< Map of username to account data

        bool _accountsDirty = false;                           ///< Flag indicating unsaved changes
        uint64_t _lastSaveTime = 0;                            ///< Timestamp of last save
        static constexpr uint64_t SAVE_INTERVAL_SECONDS = 60;  ///< Save every 60 seconds
    };

}  // namespace server
