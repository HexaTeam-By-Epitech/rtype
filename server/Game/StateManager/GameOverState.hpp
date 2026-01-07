/*
** EPITECH PROJECT, 2025
** Created by Copilot on 05/01/2026
** File description:
** GameOverState.hpp - Game finished state
*/

#pragma once

#include "common/Logger/Logger.hpp"
#include "server/Game/StateManager/GameState.hpp"

namespace server {

    /**
     * @class GameOverState
     * @brief Game state when match has ended
     */
    class GameOverState : public GameState {
       public:
        GameOverState() = default;
        ~GameOverState() override = default;

        void enter() override { LOG_INFO("✓ Entered GAME_OVER state - match ended"); }

        void exit() override { LOG_INFO("✓ Exited GAME_OVER state"); }

        void update([[maybe_unused]] float dt) override {
            // Game over - cleanup, show results
            // Prepare for next match or return to lobby
        }
    };

}  // namespace server
