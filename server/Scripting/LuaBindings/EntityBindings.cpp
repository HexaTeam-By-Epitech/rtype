/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** EntityBindings
*/

#include "EntityBindings.hpp"

#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECSWrapper/ECSWorld.tpp"

void scripting::bindings::bindEntity(sol::state &lua, ecs::wrapper::ECSWorld *world) {

    sol::usertype<ecs::wrapper::Entity> entity_type =
        lua.new_usertype<ecs::wrapper::Entity>("Entity", sol::no_constructor);

    entity_type["getAddress"] = &ecs::wrapper::Entity::getAddress;
    entity_type["isValid"] = &ecs::wrapper::Entity::isValid;

    // entity_type["setTransform"] = [](ecs::wrapper::Entity &e) -> bool {
    //     return e.with<ecs::Transform>(ecs::Transform);
    // };
    // entity_type["setVelocity"] = [](ecs::wrapper::Entity &e) -> ecs::Velocity & {
    //     return e.with<ecs::Velocity>(ecs::Velocity);
    // };
    // entity_type["setHealth"] = [](ecs::wrapper::Entity &e) -> ecs::Health & {
    //     return e.with<ecs::Health>(ecs::Health);
    // };

    entity_type["getTransform"] = [](ecs::wrapper::Entity &e) -> ecs::Transform & {
        return e.get<ecs::Transform>();
    };
    entity_type["getVelocity"] = [](ecs::wrapper::Entity &e) -> ecs::Velocity & {
        return e.get<ecs::Velocity>();
    };
    entity_type["getHealth"] = [](ecs::wrapper::Entity &e) -> ecs::Health & {
        return e.get<ecs::Health>();
    };

    entity_type["hasTransform"] = [](ecs::wrapper::Entity &e) -> bool {
        return e.has<ecs::Transform>();
    };
    entity_type["hasVelocity"] = [](ecs::wrapper::Entity &e) -> bool {
        return e.has<ecs::Velocity>();
    };
    entity_type["hasHealth"] = [](ecs::wrapper::Entity &e) -> bool {
        return e.has<ecs::Health>();
    };

    // removeComponent(addr, "Type")
    lua.set_function("removeComponent", [world](ecs::Address address, const std::string &type) -> void {
        if (type == "Transform") {
            world->getEntity(address).remove<ecs::Transform>();
        } else if (type == "Velocity") {
            world->getEntity(address).remove<ecs::Velocity>();
        } else if (type == "Health") {
            world->getEntity(address).remove<ecs::Health>();
        }
    });
}
