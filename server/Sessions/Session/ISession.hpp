/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** ISession.hpp
*/

#pragma once

#include <string>
#include <memory>

namespace server {

class ISession {
public:
    virtual ~ISession() = default;

    /**
     * @brief Get the session ID
     * @return std::string Session identifier
     */
    virtual std::string getId() const = 0;

    /**
     * @brief Check if session is active
     * @return true if active, false otherwise
     */
    virtual bool isActive() const = 0;
};

} // namespace server

