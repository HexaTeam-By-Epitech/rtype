/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** ComponentBindings - Simplified version with helper
*/

#include "ComponentBindings.hpp"
#include "ComponentBindingHelper.hpp"

#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"

namespace scripting::bindings {

    static ComponentBindingHelper g_componentHelper;

    ComponentBindingHelper &bindComponents(sol::state &lua, ecs::wrapper::ECSWorld *world) {
        (void)world;  // Parameter reserved for future use

        // Bind Transform to Lua
        auto transform_type = lua.new_usertype<ecs::Transform>(
            "Transform", sol::constructors<ecs::Transform(), ecs::Transform(float, float)>());

        transform_type["x"] = sol::property([](ecs::Transform &t) { return t.getPosition().x; },
                                            [](ecs::Transform &t, float x) {
                                                auto pos = t.getPosition();
                                                t.setPosition(x, pos.y);
                                            });

        transform_type["y"] = sol::property([](ecs::Transform &t) { return t.getPosition().y; },
                                            [](ecs::Transform &t, float y) {
                                                auto pos = t.getPosition();
                                                t.setPosition(pos.x, y);
                                            });

        transform_type["getRotation"] = &ecs::Transform::getRotation;
        transform_type["setRotation"] = &ecs::Transform::setRotation;

        // Register with helper for Entity methods generation
        g_componentHelper.registerComponent<ecs::Transform>("Transform");

        // Bind Velocity to Lua
        auto velocity_type = lua.new_usertype<ecs::Velocity>(
            "Velocity", sol::constructors<ecs::Velocity(float, float, float)>());

        velocity_type["dirX"] = sol::property([](ecs::Velocity &v) { return v.getDirection().x; },
                                              [](ecs::Velocity &v, float x) {
                                                  auto dir = v.getDirection();
                                                  v.setDirection(x, dir.y);
                                              });

        velocity_type["dirY"] = sol::property([](ecs::Velocity &v) { return v.getDirection().y; },
                                              [](ecs::Velocity &v, float y) {
                                                  auto dir = v.getDirection();
                                                  v.setDirection(dir.x, y);
                                              });

        velocity_type["speed"] = sol::property(&ecs::Velocity::getSpeed, &ecs::Velocity::setSpeed);

        // Register with helper
        g_componentHelper.registerComponent<ecs::Velocity>("Velocity");

        // Bind Health to Lua
        auto health_type = lua.new_usertype<ecs::Health>(
            "Health", sol::constructors<ecs::Health(int), ecs::Health(int, int)>());

        health_type["currentHealth"] =
            sol::property(&ecs::Health::getCurrentHealth, &ecs::Health::setCurrentHealth);

        health_type["maxHealth"] = sol::property(&ecs::Health::getMaxHealth, &ecs::Health::setMaxHealth);

        health_type["invincible"] = sol::property(&ecs::Health::isInvincible, &ecs::Health::setInvincible);

        health_type["invincibilityTimer"] =
            sol::property(&ecs::Health::getInvincibilityTimer, &ecs::Health::setInvincibilityTimer);

        // Register with helper
        g_componentHelper.registerComponent<ecs::Health>("Health");

        return g_componentHelper;
    }

}  // namespace scripting::bindings
