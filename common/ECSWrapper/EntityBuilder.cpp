/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** EntityBuilder implementation
*/

#include "EntityBuilder.hpp"

namespace ecs::wrapper {

    EntityBuilder::EntityBuilder(ECSWorld *world) : _world(world), _entity(world->createEntity()) {}

    EntityBuilder &EntityBuilder::configure(std::function<void(Entity &)> configurator) {
        configurator(_entity);
        return *this;
    }

    Entity EntityBuilder::build() {
        return _entity;
    }

    EntityBuilder::operator Entity() {
        return build();
    }

}  // namespace ecs::wrapper
