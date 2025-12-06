/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** ServerLoop.hpp
*/

#pragma once

#include "IServerLoop.hpp"

namespace server {

/**
 * @class ServerLoop
 * @brief Implementation of the main server loop
 *
 * Provides a concrete server loop implementing IServerLoop interface.
 * Currently contains only function prototypes.
 */
class ServerLoop : public IServerLoop {
public:
    ServerLoop() = default;
    ~ServerLoop() override = default;

    void start() override;
    void stop() override;
    bool isRunning() const override;

private:
    bool _running{false};
};

} // namespace server

