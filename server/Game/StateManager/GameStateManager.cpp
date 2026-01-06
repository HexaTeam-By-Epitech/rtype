/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** GameStateManager.cpp
*/

#include "server/Game/StateManager/GameStateManager.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Core/EventBus/EventBus.hpp"
#include "server/Events/GameEvent/GameEndedEvent.hpp"

namespace server {

    void GameStateManager::changeState(int stateID) {
        if (_currentStateID == stateID) {
            LOG_DEBUG("Already in state ", stateID);
            return;
        }

        // Exit current state
        if (_currentStateID >= 0 && _currentStateID < static_cast<int>(_states.size()) &&
            _states[_currentStateID]) {
            _states[_currentStateID]->exit();
            LOG_DEBUG("Exited state ", _currentStateID);
        }

        _currentStateID = stateID;

        // Enter new state
        if (_currentStateID >= 0 && _currentStateID < static_cast<int>(_states.size())) {
            if (_states[_currentStateID]) {
                _states[_currentStateID]->enter();
                LOG_INFO("✓ Changed to state ", _currentStateID);

                // Publish GameEndedEvent when entering GameOverState (state 2)
                if (_currentStateID == 2 && _eventBus) {
                    // TODO: Pass proper reason (e.g., "All players defeated", "Victory", etc.)
                    _eventBus->publish(GameEndedEvent("Game Over"));
                    LOG_INFO("[EVENT] GameEndedEvent published");
                }
            }
        } else {
            LOG_ERROR("Invalid state ID: ", stateID);
        }
    }

    int GameStateManager::getCurrentState() const {
        return _currentStateID;
    }

    void GameStateManager::registerState(int stateID, std::shared_ptr<GameState> state) {
        // Ensure vector is large enough
        if (stateID >= static_cast<int>(_states.size())) {
            _states.resize(stateID + 1);
        }
        _states[stateID] = state;
        LOG_DEBUG("Registered state ", stateID);
    }

    void GameStateManager::update(float dt) {
        if (_currentStateID >= 0 && _currentStateID < static_cast<int>(_states.size()) &&
            _states[_currentStateID]) {
            _states[_currentStateID]->update(dt);
        }
    }

    void GameStateManager::setEventBus(std::shared_ptr<EventBus> eventBus) {
        _eventBus = eventBus;
        LOG_DEBUG("GameStateManager: EventBus set");
    }

}  // namespace server