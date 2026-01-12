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
        try {
            return world->createEntity();
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to create entity: " + std::string(e.what()));
            throw;
        }
    });

    // destroyEntity(Entity) -> void
    lua.set_function("destroyEntity", [world](ecs::wrapper::Entity entity) {
        if (!world) {
            LOG_ERROR("World not set in LuaEngine");
            return;
        }
        if (!entity.isValid()) {
            LOG_WARNING("Attempted to destroy invalid entity");
            return;
        }
        try {
            world->destroyEntity(entity);
        } catch (const std::exception &e) {
            LOG_ERROR("Failed to destroy entity: " + std::string(e.what()));
        }
    });

    // log(string message) -> void
    lua.set_function("log", [](const std::string &message) { LOG_DEBUG("[LUA] " + message); });

    // entityExists(addr) -> bool
    lua.set_function("entityExists", [world](ecs::Address addr) -> bool {
        if (!world) {
            LOG_ERROR("World not set in LuaEngine");
            return false;
        }
        try {
            auto entity = world->getEntity(addr);
            return entity.isValid();
        } catch (const std::exception &e) {
            LOG_ERROR("Error checking entity existence: " + std::string(e.what()));
            return false;
        }
    });
}
