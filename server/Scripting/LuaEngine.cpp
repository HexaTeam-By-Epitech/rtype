/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaEngine implementation
*/

#include "LuaEngine.hpp"
#include <filesystem>
#include "LuaBindings/EntityBindings.hpp"
#include "LuaBindings/RegistryBindings.hpp"
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"
#include "common/Logger/Logger.hpp"

namespace scripting {
    LuaEngine::LuaEngine(const std::string &scriptPath) : _scriptPath(scriptPath), _world(nullptr) {
        _lua.open_libraries(sol::lib::base, sol::lib::package, sol::lib::math, sol::lib::table,
                            sol::lib::string);

        initializeBindings();
        LOG_INFO("LuaEngine initialized with script path: " + scriptPath);
    }

    void LuaEngine::setWorld(ecs::wrapper::ECSWorld *world) {
        _world = world;
    }

    void LuaEngine::initializeBindings() {
        bindComponents();
        bindEntity();
        bindWorld();
    }

    void LuaEngine::bindComponents() {
        // Binding Transform
        auto transform_type = _lua.new_usertype<ecs::Transform>(
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
        auto velocity_type = _lua.new_usertype<ecs::Velocity>(
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

        // Binding Health
        auto health_type = _lua.new_usertype<ecs::Health>(
            "Health", sol::constructors<ecs::Health(int), ecs::Health(int, int)>());
        health_type["getCurrentHealth"] = &ecs::Health::getCurrentHealth;
        health_type["getMaxHealth"] = &ecs::Health::getMaxHealth;
        health_type["setCurrentHealth"] = &ecs::Health::setCurrentHealth;
        health_type["isInvincible"] = &ecs::Health::isInvincible;
        health_type["setInvincible"] = &ecs::Health::setInvincible;
    };

    void LuaEngine::bindEntity() {
        // Binding Entity wrapper
        auto entity_type = _lua.new_usertype<ecs::wrapper::Entity>("Entity", sol::no_constructor);

        entity_type["getAddress"] = &ecs::wrapper::Entity::getAddress;
        entity_type["isValid"] = &ecs::wrapper::Entity::isValid;

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
    }

    void LuaEngine::bindWorld() {
        _lua.set_function("createEntity", [this]() -> ecs::wrapper::Entity {
            if (!_world) {
                LOG_ERROR("World not set in LuaEngine");
                throw std::runtime_error("World not initialized");
            }
            return _world->createEntity();
        });

        _lua.set_function("destroyEntity", [this](ecs::wrapper::Entity entity) {
            if (!_world) {
                LOG_ERROR("World not set in LuaEngine");
                return;
            }
            _world->destroyEntity(entity);
        });

        _lua.set_function("log", [](const std::string &message) { LOG_INFO("[LUA] " + message); });
    };

    bool LuaEngine::loadScript(const std::string &scriptPath) {
        std::string fullPath = _scriptPath + scriptPath;

        if (!std::filesystem::exists(fullPath)) {
            LOG_ERROR("Lua script not found: " + fullPath);
            return false;
        }

        try {
            sol::table script = _lua.script_file(fullPath);
            _scriptCache[scriptPath] = script;
            LOG_INFO("Loaded Lua script: " + scriptPath);
            return true;
        } catch (const sol::error &e) {
            LOG_ERROR("Lua error loading " + scriptPath + ": " + std::string(e.what()));
            return false;
        }
    }

    void LuaEngine::executeUpdate(const std::string &scriptPath, ecs::wrapper::Entity entity,
                                  float deltaTime) {
        if (!_world) {
            LOG_ERROR("World not set in LuaEngine");
            return;
        }

        if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
            if (!loadScript(scriptPath)) {
                return;
            }
        }

        try {
            sol::table script = _scriptCache[scriptPath];
            sol::optional<sol::function> onUpdate = script["onUpdate"];

            if (onUpdate) {
                onUpdate.value()(entity, deltaTime);
            } else {
                LOG_WARNING("Script " + scriptPath + " has no onUpdate function");
            }
        } catch (const sol::error &e) {
            LOG_ERROR("Lua runtime error in " + scriptPath + ": " + std::string(e.what()));
        }
    }

    template <typename... Args>
    void LuaEngine::callFunction(const std::string &scriptPath, const std::string &functionName,
                                 Args &&...args) {
        if (_scriptCache.find(scriptPath) == _scriptCache.end()) {
            if (!loadScript(scriptPath)) {
                return;
            }
        }

        try {
            sol::table script = _scriptCache[scriptPath];
            sol::optional<sol::function> func = script[functionName];

            if (func) {
                func.value()(std::forward<Args>(args)...);
            } else {
                LOG_WARNING("Function " + functionName + " not found in " + scriptPath);
            }
        } catch (const sol::error &e) {
            LOG_ERROR("Lua error calling " + functionName + ": " + std::string(e.what()));
        }
    }
};  // namespace scripting
