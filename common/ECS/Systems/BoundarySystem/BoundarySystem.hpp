/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** BoundarySystem
*/

#ifndef BOUNDARYSYSTEM_HPP_
#define BOUNDARYSYSTEM_HPP_

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
        BoundarySystem() = default;
        ~BoundarySystem() override = default;
    };
}

#endif
