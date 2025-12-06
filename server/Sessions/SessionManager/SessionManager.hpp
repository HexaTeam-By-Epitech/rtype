/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** SessionManager.hpp
*/

#pragma once

#include "ISessionManager.hpp"
#include <unordered_map>

namespace server {

class SessionManager : public ISessionManager {
public:
    SessionManager() = default;
    ~SessionManager() override = default;

    std::shared_ptr<Session> createSession(const std::string &id) override;
    std::shared_ptr<Session> getSession(const std::string &id) override;
    void removeSession(const std::string &id) override;

private:
    std::unordered_map<std::string, std::shared_ptr<Session>> _sessions;
};

} // namespace server

