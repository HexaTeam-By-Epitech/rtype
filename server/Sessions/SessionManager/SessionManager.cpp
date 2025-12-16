/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** SessionManager.cpp
*/

#include "server/Sessions/SessionManager/SessionManager.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    SessionManager::SessionManager() : _authService(std::make_shared<AuthService>()) {}

    SessionManager::SessionManager(std::shared_ptr<AuthService> authService) : _authService(authService) {}

    std::string SessionManager::authenticateAndCreateSession(const std::string &username,
                                                             const std::string &password) {
        if (!_authService->authenticate(username, password)) {
            LOG_WARNING("Authentication failed for user: ", username);
            return "";
        }

        // Generate token and create session
        std::string token = _authService->generateToken(username);
        std::shared_ptr<Session> session = createSession(token);

        LOG_INFO("✓ User authenticated and session created: ", username);
        return token;
    }

    std::shared_ptr<Session> SessionManager::createSession(const std::string &id) {
        if (_sessions.find(id) != _sessions.end()) {
            LOG_WARNING("Session ", id, " already exists");
            return _sessions[id];
        }

        std::shared_ptr<Session> session = std::make_shared<Session>(id);
        _sessions[id] = session;

        LOG_INFO("✓ Session created: ", id);
        return session;
    }

    std::shared_ptr<Session> SessionManager::getSession(const std::string &id) {
        auto it = _sessions.find(id);
        if (it != _sessions.end()) {
            return it->second;
        }
        return nullptr;
    }

    void SessionManager::removeSession(const std::string &id) {
        auto it = _sessions.find(id);
        if (it != _sessions.end()) {
            _sessions.erase(it);
            LOG_INFO("✓ Session removed: ", id);
        }
    }

}  // namespace server
