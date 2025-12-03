/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ISystem
*/

#ifndef ISYSTEM_HPP_
#define ISYSTEM_HPP_

namespace ecs {
    /**
     * @class ISystem
     * @brief Base interface for all ECS systems.
     * 
     * All systems in the Entity-Component-System must inherit from this interface.
     * Systems contain the game logic and operate on entities with specific component combinations.
     * 
     * @note Systems should be stateless when possible.
     * @note Logic processes entities based on their component composition.
     */
    class ISystem {
       public:
        /**
         * @brief Virtual destructor.
         */
        virtual ~ISystem() = default;
    };
}

#endif
