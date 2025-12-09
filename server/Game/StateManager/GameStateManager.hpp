/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameStateManager.hpp
*/

#pragma once

#include <memory>
#include <vector>
#include "GameState.hpp"
#include "IGameStateManager.hpp"

namespace server {

    /**
     * @class GameStateManager
     * @brief Handles switching between game states
     */
    class GameStateManager : public IGameStateManager {
       public:
        GameStateManager() = default;
        ~GameStateManager() override = default;

        // Prototypes
        void changeState(int stateID) override;
        int getCurrentState() const override;

       private:
        std::vector<std::shared_ptr<GameState>> _states;
        int _currentStateID{-1};
    };

}  // namespace server
