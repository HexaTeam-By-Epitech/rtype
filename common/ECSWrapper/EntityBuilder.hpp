/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** EntityBuilder - Fluent builder pattern for entity creation
*/

#pragma once

#include <functional>
#include <memory>
#include "ECSWorld.hpp"

namespace ecs::wrapper {

    /**
     * @class EntityBuilder
     * @brief Builder pattern for creating entities with multiple components.
     * 
     * Provides a fluent interface for constructing complex entities.
     * Useful for creating game objects with multiple components in a clean way.
     */
    class EntityBuilder {
       private:
        ECSWorld *_world;
        Entity _entity;

       public:
        /**
         * @brief Construct an EntityBuilder.
         * 
         * @param world Pointer to the ECS world
         */
        explicit EntityBuilder(ECSWorld *world);

        /**
         * @brief Add a component to the entity being built.
         * 
         * @tparam T Component type
         * @param component The component data
         * @return EntityBuilder& Reference for chaining
         */
        template <typename T>
        EntityBuilder &with(const T &component);

        /**
         * @brief Add a component using a factory function.
         * 
         * @tparam T Component type
         * @param factory Function that creates the component
         * @return EntityBuilder& Reference for chaining
         */
        template <typename T>
        EntityBuilder &with(std::function<T()> factory);

        /**
         * @brief Apply a configuration function to the entity.
         * 
         * @param configurator Function that configures the entity
         * @return EntityBuilder& Reference for chaining
         * 
         * @code
         * builder.configure([](Entity e) {
         *     e.get<Transform>().position = {100, 200};
         * });
         * @endcode
         */
        EntityBuilder &configure(std::function<void(Entity &)> configurator);

        /**
         * @brief Finalize and return the built entity.
         * 
         * @return Entity The constructed entity
         */
        Entity build();

        /**
         * @brief Implicit conversion to Entity.
         */
        operator Entity();
    };

}  // namespace ecs::wrapper

#include "EntityBuilder.tpp"
