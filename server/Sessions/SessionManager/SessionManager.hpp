/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** SessionManager.hpp
*/

#pragma once

#include <memory>
#include <unordered_map>
#include "server/Sessions/Auth/AuthService.hpp"
#include "server/Sessions/SessionManager/ISessionManager.hpp"

namespace server {

    /**
     * @class SessionManager
     * @brief Manages active player sessions with authentication support
     */
    class SessionManager : public ISessionManager {
       public:
        SessionManager();
        explicit SessionManager(std::shared_ptr<AuthService> authService);
        ~SessionManager() override = default;

        std::shared_ptr<Session> createSession(const std::string &id) override;
        std::shared_ptr<Session> getSession(const std::string &id) override;
        void removeSession(const std::string &id) override;

        /**
         * @brief Authenticate and create a session
         * @param username Username
         * @param password Password
         * @return Session ID if successful, empty string otherwise
         */
        std::string authenticateAndCreateSession(const std::string &username, const std::string &password);

        /**
         * @brief Get the auth service
         * @return Shared pointer to AuthService
         */
        std::shared_ptr<AuthService> getAuthService() { return _authService; }

       private:
        std::unordered_map<std::string, std::shared_ptr<Session>> _sessions;
        std::shared_ptr<AuthService> _authService;
    };

}  // namespace server
