/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** BoundarySystem
*/

#pragma once

#include "../../Components/IComponent.hpp"
#include "../../Components/Transform.hpp"
#include "../ISystem.hpp"

namespace ecs {
    /**
     * @class BoundarySystem
     * @brief System managing entity boundaries and screen limits.
     * 
     * Handles entities going out of bounds and applies boundary constraints.
     * Requires Transform component.
     */
    class BoundarySystem : public ISystem {
       public:
        /**
         * @brief Constructs a BoundarySystem with specified screen dimensions.
         * 
         * @param screenWidth Width of the screen/game area (default: 1920)
         * @param screenHeight Height of the screen/game area (default: 1080)
         */
        BoundarySystem(int screenWidth = 1920, int screenHeight = 1080);

        /**
         * @brief Default destructor.
         */
        ~BoundarySystem() override = default;

        /**
         * @brief Checks entities against screen boundaries.
         * 
         * Destroys entities that have moved outside the screen bounds
         * (with a 100-pixel margin). Typically used for projectiles and enemies.
         * 
         * @param registry Reference to the ECS registry
         * @param deltaTime Time elapsed since last frame (in seconds)
         */
        void update(Registry &registry, float deltaTime) override;

        /**
         * @brief Gets the component mask for this system.
         * 
         * @return ComponentMask requiring Transform component
         */
        ComponentMask getComponentMask() const override;

        /**
         * @brief Updates the screen dimensions.
         * 
         * Allows dynamic adjustment of boundary limits when screen is resized.
         * 
         * @param width New screen width
         * @param height New screen height
         */
        void setScreenSize(int width, int height);

       private:
        int _screenWidth;
        int _screenHeight;
    };
}
