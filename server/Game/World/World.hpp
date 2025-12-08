/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** World.hpp
*/

#pragma once

#include <vector>
#include "IWorld.hpp"

namespace server {

    /**
     * @class World
     * @brief Concrete implementation of IWorld
     */
    class World : public IWorld {
       public:
        World() = default;
        ~World() override = default;

        // Prototypes
        void addEntity(int entityID) override;
        void removeEntity(int entityID) override;
        void update(float dt) override;

       private:
        std::vector<int> _entities;
    };

}  // namespace server
