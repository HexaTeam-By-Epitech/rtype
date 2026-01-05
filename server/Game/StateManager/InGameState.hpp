/*
** EPITECH PROJECT, 2025
** Created by Copilot on 05/01/2026
** File description:
** InGameState.hpp - Active gameplay state
*/

#pragma once

#include "common/Logger/Logger.hpp"
#include "server/Game/StateManager/GameState.hpp"

namespace server {

    /**
     * @class InGameState
     * @brief Game state when match is actively running
     */
    class InGameState : public GameState {
       public:
        InGameState() = default;
        ~InGameState() override = default;

        void enter() override { LOG_INFO("✓ Entered IN_GAME state - match started!"); }

        void exit() override { LOG_INFO("✓ Exited IN_GAME state"); }

        void update(float dt) override {
            // Active game - systems are running
            // This is called from GameLogic::update()
            (void)dt;  // Systems updated separately
        }
    };

}  // namespace server
