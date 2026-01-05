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

        /**
         * @brief Add entity to world tracking
         * @param entityID Entity identifier
         */
        virtual void addEntity(int entityID) = 0;

        /**
         * @brief Remove entity from world tracking
         * @param entityID Entity identifier
         */
        virtual void removeEntity(int entityID) = 0;

        /**
         * @brief Update all systems in the world
         * @param dt Delta time in seconds
         */
        virtual void update(float dt) = 0;
    };

}  // namespace server
