/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IGameStateManager.hpp
*/

#pragma once

#include <memory>

namespace server {

/**
 * @class IGameStateManager
 * @brief Interface for managing game states
 */
class IGameStateManager {
public:
    virtual ~IGameStateManager() = default;

    // Prototypes
    virtual void changeState(int stateID) = 0;
    virtual int getCurrentState() const = 0;
};

} // namespace server

