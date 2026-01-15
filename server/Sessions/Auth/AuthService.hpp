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

    struct AccountData {
        std::string username;
        std::string passwordHash;  // bcrypt hash
        uint64_t createdAt;        // timestamp
        uint64_t lastLogin;        // timestamp
    };

    class AuthService : public IAuthService {
       public:
        AuthService();
        explicit AuthService(const std::string &accountsFile);
        ~AuthService() override = default;

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
        /**
         * @brief Hash a password using bcrypt
         * @param password Plaintext password
         * @return std::string Bcrypt hash
         */
        std::string hashPassword(const std::string &password);

        /**
         * @brief Verify a password against a bcrypt hash
         * @param password Plaintext password
         * @param hash Bcrypt hash to verify against
         * @return bool True if password matches
         */
        bool verifyPassword(const std::string &password, const std::string &hash);

        std::unordered_set<std::string> _authenticatedUsers;         ///< Set of authenticated usernames
        std::unordered_map<std::string, std::string> _activeTokens;  ///< Map of tokens to usernames
        std::unordered_map<std::string, AccountData> _accounts;      ///< Map of username to account data
        std::string _accountsFile = "accounts.json";                 ///< JSON file to store accounts
    };

}  // namespace server
