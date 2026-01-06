/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** ComponentBindings
*/

#include "ComponentBindings.hpp"

#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"

void scripting::bindings::bindComponents(sol::state &lua, ecs::wrapper::ECSWorld *world) {
    // Binding Transform
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

    // Binding Velocity
    auto velocity_type =
        lua.new_usertype<ecs::Velocity>("Velocity", sol::constructors<ecs::Velocity(float, float, float)>());
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

    // Binding Health
    auto health_type =
        lua.new_usertype<ecs::Health>("Health", sol::constructors<ecs::Health(int), ecs::Health(int, int)>());
    health_type["getCurrentHealth"] = &ecs::Health::getCurrentHealth;
    health_type["getMaxHealth"] = &ecs::Health::getMaxHealth;
    health_type["setCurrentHealth"] = &ecs::Health::setCurrentHealth;
    health_type["isInvincible"] = &ecs::Health::isInvincible;
    health_type["setInvincible"] = &ecs::Health::setInvincible;
};
