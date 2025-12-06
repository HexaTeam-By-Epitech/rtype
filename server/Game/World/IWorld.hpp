/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IWorld.hpp
*/

#pragma once

namespace server {

/**
 * @class IWorld
 * @brief Interface for the game world
 */
class IWorld {
public:
    virtual ~IWorld() = default;

    // Prototypes
    virtual void addEntity(int entityID) = 0;
    virtual void removeEntity(int entityID) = 0;
    virtual void update(float dt) = 0;
};

} // namespace server

