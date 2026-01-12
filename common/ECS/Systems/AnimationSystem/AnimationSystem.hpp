/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** AnimationSystem
*/

#pragma once

#include "../../Components/Animation.hpp"
#include "../../Components/AnimationSet.hpp"
#include "../../Components/IComponent.hpp"
#include "../../Components/Sprite.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class AnimationSystem
     * @brief System managing sprite animation playback.
     * 
     * Updates animation timers, advances frames, handles looping and transitions.
     * Updates Sprite components with current animation frame rectangles.
     * Requires Animation, AnimationSet, and Sprite components.
     */
    class AnimationSystem : public ISystem {
       public:
        /**
         * @brief Default constructor.
         */
        AnimationSystem() = default;

        /**
         * @brief Default destructor.
         */
        ~AnimationSystem() override = default;

        /**
         * @brief Updates all entity animations.
         * 
         * Performs the following operations per entity:
         * - Advances animation timer based on deltaTime
         * - Changes animation frame when timer exceeds frame duration
         * - Handles animation looping and stopping
         * - Transitions to next animation clip if specified
         * - Updates Sprite sourceRect to display current frame
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Animation, AnimationSet, and Sprite components
         */
        ComponentMask getComponentMask() const override;
    };
}  // namespace ecs
