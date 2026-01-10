/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** EntityBindings - Simplified version with helper
*/

#include "EntityBindings.hpp"

void scripting::bindings::bindEntity(sol::state &lua, ecs::wrapper::ECSWorld *world,
                                     ComponentBindingHelper &helper) {
    // Create the Entity usertype
    sol::usertype<ecs::wrapper::Entity> entity_type =
        lua.new_usertype<ecs::wrapper::Entity>("Entity", sol::no_constructor);

    // Basic Entity methods
    entity_type["getAddress"] = &ecs::wrapper::Entity::getAddress;
    entity_type["isValid"] = &ecs::wrapper::Entity::isValid;

    helper.applyEntityMethods(entity_type);

    // Automatically generate the removeComponent function
    helper.applyRemoveFunction(lua, world);

    LOG_INFO("Entity bindings initialized with error handling");
}
