/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** OrbitalSystem - Updates orbital module positions
*/

#pragma once

#include "../../Components/OrbitalModule.hpp"
#include "../../Components/Transform.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class OrbitalSystem
     * @brief System managing orbital module movement around parent entities.
     * 
     * Updates positions of entities with OrbitalModule components to orbit
     * around their parent entities. Uses circular motion with constant radius.
     * Requires OrbitalModule and Transform components.
     */
    class OrbitalSystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        OrbitalSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~OrbitalSystem() override = default;

        /**
         * @brief Updates all orbital module positions.
         * 
         * For each orbital module:
         * 1. Finds parent entity position
         * 2. Updates orbital angle based on speed and deltaTime
         * 3. Calculates new position using circular motion
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring OrbitalModule and Transform components
         */
        ComponentMask getComponentMask() const override;

       private:
        /**
         * @brief Updates a single orbital module's position.
         * 
         * @param registry Reference to the ECS registry
         * @param moduleEntity Entity ID of the orbital module
         * @param orbital OrbitalModule component
         * @param transform Transform component to update
         * @param deltaTime Time elapsed since last frame
         */
        void updateOrbitalPosition(Registry &registry, Address moduleEntity, OrbitalModule &orbital,
                                   Transform &transform, float deltaTime);
    };
}  // namespace ecs
