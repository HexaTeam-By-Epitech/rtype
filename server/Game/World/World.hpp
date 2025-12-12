/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** World.hpp
*/

#pragma once

#include <memory>
#include <vector>
#include "../../../common/ECS/Registry.hpp"
#include "IWorld.hpp"

namespace server {

    /**
     * @class World
     * @brief Concrete implementation of IWorld wrapping ECS Registry
     * 
     * Provides high-level entity management layer above raw Registry.
     * Used by GameLogic to manage entities in a deterministic way.
     */
    class World : public IWorld {
       public:
        /**
         * @brief Constructor
         * @param registry ECS registry to wrap
         */
        explicit World(std::shared_ptr<ecs::Registry> registry);

        ~World() override = default;

        // IWorld interface implementation
        void addEntity(int entityID) override;
        void removeEntity(int entityID) override;
        void update(float dt) override;

        /**
         * @brief Get the underlying ECS registry
         * @return Shared pointer to Registry
         */
        std::shared_ptr<ecs::Registry> getRegistry() { return _registry; }

        /**
         * @brief Get number of active entities
         * @return Entity count
         */
        size_t getEntityCount() const { return _entities.size(); }

       private:
        std::shared_ptr<ecs::Registry> _registry;
        std::vector<int> _entities;
    };

}  // namespace server
