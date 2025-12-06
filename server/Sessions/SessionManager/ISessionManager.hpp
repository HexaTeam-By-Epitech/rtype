/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** ISessionManager.hpp
*/

#pragma once

#include "Session.hpp"
#include <memory>
#include <string>

namespace server {

class ISessionManager {
public:
    virtual ~ISessionManager() = default;

    /**
     * @brief Create a new session
     * @param id Session identifier
     * @return std::shared_ptr<Session>
     */
    virtual std::shared_ptr<Session> createSession(const std::string &id) = 0;

    /**
     * @brief Get session by ID
     * @param id Session identifier
     * @return std::shared_ptr<Session>
     */
    virtual std::shared_ptr<Session> getSession(const std::string &id) = 0;

    /**
     * @brief Remove session by ID
     * @param id Session identifier
     */
    virtual void removeSession(const std::string &id) = 0;
};

} // namespace server

