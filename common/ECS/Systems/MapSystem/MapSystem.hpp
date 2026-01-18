/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** MapSystem.hpp - System managing map scrolling, transitions and state
*/

#pragma once

#include "../../Components/MapData.hpp"
#include "../../Components/Transform.hpp"
#include "../ISystem.hpp"

namespace ecs {

    /**
     * @class MapSystem
     * @brief System managing map state, scrolling, and transitions.
     * 
     * Handles:
     * - Automatic horizontal scrolling of entities based on map speed
     * - Map duration tracking and completion detection
     * - Transition events when maps are completed
     * - Background entity management
     * 
     * Works with entities that have MapData component.
     */
    class MapSystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        MapSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~MapSystem() override = default;

        /**
         * @brief Updates map state and handles scrolling.
         * 
         * - Updates elapsed time on active maps
         * - Applies scrolling to all entities with Transform (except players)
         * - Detects map completion based on duration
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring MapData component
         */
        ComponentMask getComponentMask() const override;

       private:
        /**
         * @brief Apply scrolling to entities (moves them left based on map speed).
         * @param registry Reference to the ECS registry
         * @param scrollSpeed Horizontal scroll speed in pixels/second
         * @param deltaTime Time elapsed since last frame
         */
        void _applyScrolling(Registry &registry, float scrollSpeed, float deltaTime);
    };

}  // namespace ecs
