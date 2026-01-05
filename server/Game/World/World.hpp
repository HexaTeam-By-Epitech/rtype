/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** World.hpp
*/

#pragma once

#include <memory>
#include <vector>
#include "common/ECSWrapper/ECSWorld.hpp"
#include "server/Game/World/IWorld.hpp"

namespace server {

    /**
     * @class World
     * @brief Concrete implementation of IWorld wrapping ECS wrapper
     * 
     * Provides high-level entity management layer using ECSWorld wrapper.
     * Used by GameLogic to manage entities in a deterministic way.
     */
    class World : public IWorld {
       public:
        /**
         * @brief Constructor
         * @param ecsWorld ECSWorld wrapper instance
         */
        explicit World(std::shared_ptr<ecs::wrapper::ECSWorld> ecsWorld);

        ~World() override = default;

        // IWorld interface implementation
        void addEntity(int entityID) override;
        void removeEntity(int entityID) override;
        void update(float dt) override;

        /**
         * @brief Get the underlying ECS world
         * @return Shared pointer to ECSWorld
         */
        std::shared_ptr<ecs::wrapper::ECSWorld> getECSWorld() { return _ecsWorld; }

        /**
         * @brief Get number of active entities
         * @return Entity count
         */
        size_t getEntityCount() const { return _entities.size(); }

       private:
        std::shared_ptr<ecs::wrapper::ECSWorld> _ecsWorld;
        std::vector<int> _entities;
    };

}  // namespace server