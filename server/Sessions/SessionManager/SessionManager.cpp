/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** SessionManager.cpp
*/

#include "server/Sessions/SessionManager/SessionManager.hpp"
#include "common/Logger/Logger.hpp"

namespace server {

    std::shared_ptr<Session> SessionManager::createSession(const std::string &id) {
        if (_sessions.find(id) != _sessions.end()) {
            LOG_WARNING("Session ", id, " already exists");
            return _sessions[id];
        }

        auto session = std::make_shared<Session>(id);
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
