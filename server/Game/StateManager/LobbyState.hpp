/*
** EPITECH PROJECT, 2025
** Created by Copilot on 05/01/2026
** File description:
** LobbyState.hpp - Waiting for players state
*/

#pragma once

#include "common/Logger/Logger.hpp"
#include "server/Game/StateManager/GameState.hpp"

namespace server {

    /**
     * @class LobbyState
     * @brief Game state when waiting for players in lobby
     */
    class LobbyState : public GameState {
       public:
        LobbyState() = default;
        ~LobbyState() override = default;

        void enter() override { LOG_INFO("✓ Entered LOBBY state - waiting for players..."); }

        void exit() override { LOG_INFO("✓ Exited LOBBY state"); }

        void update([[maybe_unused]] float dt) override {
            // In lobby, don't update game systems
            // Just wait for enough players to start
        }
    };

}  // namespace server
