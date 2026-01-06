/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** WorldBindings
*/

#include "WorldBindings.hpp"

void scripting::bindings::bindWorld(sol::state &lua, ecs::wrapper::ECSWorld *world) {

    // createEntity -> Entity
    lua.set_function("createEntity", [world]() -> ecs::wrapper::Entity {
        if (!world) {
            LOG_ERROR("World not set in LuaEngine");
            throw std::runtime_error("World not initialized");
        }
        return world->createEntity();
    });

    // destroyEntity(Entity) -> void
    lua.set_function("destroyEntity", [world](ecs::wrapper::Entity entity) {
        if (!world) {
            LOG_ERROR("World not set in LuaEngine");
            return;
        }
        world->destroyEntity(entity);
    });

    // log(string message) -> void
    lua.set_function("log", [](const std::string &message) { LOG_INFO("[LUA] " + message); });

    // entityExists(addr) -> bool
    lua.set_function("entityExists",
                     [world](ecs::Address addr) -> bool { return (world->getEntity(addr) != 0U); });
}
