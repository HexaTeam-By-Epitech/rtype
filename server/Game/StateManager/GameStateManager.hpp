/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameStateManager.hpp
*/

#pragma once

#include <memory>
#include <vector>
#include "server/Game/StateManager/GameState.hpp"
#include "server/Game/StateManager/IGameStateManager.hpp"

namespace server {

    /**
     * @class GameStateManager
     * @brief Handles switching between game states
     */
    class GameStateManager : public IGameStateManager {
       public:
        GameStateManager() = default;
        ~GameStateManager() override = default;

        // IGameStateManager interface
        void changeState(int stateID) override;
        int getCurrentState() const override;

        /**
         * @brief Register a game state
         * @param stateID State identifier
         * @param state Shared pointer to GameState
         */
        void registerState(int stateID, std::shared_ptr<GameState> state);

        /**
         * @brief Update current state
         * @param dt Delta time
         */
        void update(float dt);

       private:
        std::vector<std::shared_ptr<GameState>> _states;
        int _currentStateID{-1};
    };

}  // namespace server
