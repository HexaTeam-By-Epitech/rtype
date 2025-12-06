/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameStateManager.hpp
*/

#pragma once

#include "IGameStateManager.hpp"
#include "GameState.hpp"
#include <vector>
#include <memory>

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
    int _currentStateID;
};

} // namespace server

