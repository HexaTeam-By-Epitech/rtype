/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ISystem
*/

#pragma once

#include <cstdint>
#include "../Registry.hpp"

namespace ecs {
    // Placeholder declaration

    /**
     * @brief Type alias for component bitmask.
     * 
     * Used to represent which components are required by a system.
     * Each bit corresponds to a component type ID.
     * 
     * @note Size matches Registry::Signature (N_MAX_COMPONENTS = 64 bits)
     */
    using ComponentMask = std::uint64_t;

    /**
     * @class ISystem
     * @brief Base interface for all ECS systems.
     * 
     * All systems in the Entity-Component-System must inherit from this interface.
     * Systems contain the game logic and operate on entities with specific component combinations.
     * 
     * @note Systems should be stateless when possible.
     * @note Logic processes entities based on their component composition.
     * @note Each system defines which components it requires via getComponentMask().
     */
    class ISystem {
       public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~ISystem() = default;

        /**
         * @brief Update the system logic for one frame.
         * 
         * Called every frame by the game loop. The system processes all entities
         * that match its component requirements.
         * 
         * @param registry Reference to the ECS registry containing all entities
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        virtual void update(Registry &registry, float deltaTime) = 0;

        /**
         * @brief Get the bitmask of required components.
         * 
         * Returns a bitmask indicating which components an entity must have
         * to be processed by this system. The Registry uses this mask to filter entities.
         * 
         * @return ComponentMask Bitmask of required component types
         * 
         */
        virtual ComponentMask getComponentMask() const = 0;
    };
}  // namespace ecs
