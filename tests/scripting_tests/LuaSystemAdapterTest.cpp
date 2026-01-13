/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** LuaSystemAdapterTest - Tests for Lua system integration with ECS
*/

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Registry.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"
#include "server/Scripting/LuaEngine.hpp"
#include "server/Scripting/LuaSystemAdapter.hpp"

class LuaSystemAdapterTest : public ::testing::Test {
   protected:
    void SetUp() override {
        world = std::make_unique<ecs::wrapper::ECSWorld>();
        luaEngine = std::make_unique<scripting::LuaEngine>("./test_scripts/");
        luaEngine->setWorld(world.get());
        luaSystem = std::make_unique<scripting::LuaSystemAdapter>(luaEngine.get(), world.get());

        // Create test scripts directory
        std::filesystem::create_directories("./test_scripts");
    }

    void TearDown() override {
        // Clean up test scripts
        std::filesystem::remove_all("./test_scripts");
    }

    void createTestScript(const std::string &filename, const std::string &content) {
        std::ofstream file("./test_scripts/" + filename);
        file << content;
        file.close();
    }

    std::unique_ptr<ecs::wrapper::ECSWorld> world;
    std::unique_ptr<scripting::LuaEngine> luaEngine;
    std::unique_ptr<scripting::LuaSystemAdapter> luaSystem;
};

// ========== System Integration Tests ==========

TEST_F(LuaSystemAdapterTest, SystemProcessesEntitiesWithLuaScript) {
    createTestScript("move_right.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = transform.x + 100 * deltaTime
            end
        end
    )");

    auto &registry = world->getRegistry();
    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));
    entity.with(ecs::LuaScript("move_right.lua"));

    luaSystem->update(registry, 0.016f);

    auto &transform = entity.get<ecs::Transform>();
    EXPECT_FLOAT_EQ(transform.getPosition().x, 1.6f);  // 100 * 0.016
}

TEST_F(LuaSystemAdapterTest, SystemIgnoresEntitiesWithoutLuaScript) {
    auto &registry = world->getRegistry();
    auto entity = world->createEntity();
    entity.with(ecs::Transform(100.0f, 200.0f));
    // No LuaScript component

    luaSystem->update(registry, 0.016f);

    auto &transform = entity.get<ecs::Transform>();
    EXPECT_FLOAT_EQ(transform.getPosition().x, 100.0f);  // Unchanged
    EXPECT_FLOAT_EQ(transform.getPosition().y, 200.0f);
}

TEST_F(LuaSystemAdapterTest, SystemProcessesMultipleEntities) {
    createTestScript("increment.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = transform.x + 1
            end
        end
    )");

    auto &registry = world->getRegistry();

    auto entity1 = world->createEntity();
    entity1.with(ecs::Transform(0.0f, 0.0f));
    entity1.with(ecs::LuaScript("increment.lua"));

    auto entity2 = world->createEntity();
    entity2.with(ecs::Transform(10.0f, 0.0f));
    entity2.with(ecs::LuaScript("increment.lua"));

    auto entity3 = world->createEntity();
    entity3.with(ecs::Transform(20.0f, 0.0f));
    entity3.with(ecs::LuaScript("increment.lua"));

    luaSystem->update(registry, 0.016f);

    EXPECT_FLOAT_EQ(entity1.get<ecs::Transform>().getPosition().x, 1.0f);
    EXPECT_FLOAT_EQ(entity2.get<ecs::Transform>().getPosition().x, 11.0f);
    EXPECT_FLOAT_EQ(entity3.get<ecs::Transform>().getPosition().x, 21.0f);
}

TEST_F(LuaSystemAdapterTest, SystemHandlesDifferentScripts) {
    createTestScript("move_right.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = transform.x + 10
            end
        end
    )");

    createTestScript("move_down.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.y = transform.y + 10
            end
        end
    )");

    auto &registry = world->getRegistry();

    auto entity1 = world->createEntity();
    entity1.with(ecs::Transform(0.0f, 0.0f));
    entity1.with(ecs::LuaScript("move_right.lua"));

    auto entity2 = world->createEntity();
    entity2.with(ecs::Transform(0.0f, 0.0f));
    entity2.with(ecs::LuaScript("move_down.lua"));

    luaSystem->update(registry, 0.016f);

    auto &transform1 = entity1.get<ecs::Transform>();
    auto &transform2 = entity2.get<ecs::Transform>();

    EXPECT_FLOAT_EQ(transform1.getPosition().x, 10.0f);
    EXPECT_FLOAT_EQ(transform1.getPosition().y, 0.0f);

    EXPECT_FLOAT_EQ(transform2.getPosition().x, 0.0f);
    EXPECT_FLOAT_EQ(transform2.getPosition().y, 10.0f);
}

TEST_F(LuaSystemAdapterTest, ComponentMaskIncludesLuaScript) {
    auto mask = luaSystem->getComponentMask();
    auto luaScriptBit = ecs::getComponentType<ecs::LuaScript>();

    EXPECT_TRUE((mask & (1ULL << luaScriptBit)) != 0);
}

// ========== Edge Cases ==========

TEST_F(LuaSystemAdapterTest, EmptyScriptPathIgnored) {
    auto &registry = world->getRegistry();
    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));
    entity.with(ecs::LuaScript(""));  // Empty script path

    // Should not crash
    EXPECT_NO_THROW(luaSystem->update(registry, 0.016f));
}

TEST_F(LuaSystemAdapterTest, NonExistentScriptHandledGracefully) {
    auto &registry = world->getRegistry();
    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));
    entity.with(ecs::LuaScript("non_existent.lua"));

    // Should log error but not crash
    EXPECT_NO_THROW(luaSystem->update(registry, 0.016f));
}

TEST_F(LuaSystemAdapterTest, ScriptErrorDoesNotStopOtherEntities) {
    createTestScript("error.lua", R"(
        function onUpdate(entity, deltaTime)
            error("Intentional error")
        end
    )");

    createTestScript("safe.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = 999
            end
        end
    )");

    auto &registry = world->getRegistry();

    auto errorEntity = world->createEntity();
    errorEntity.with(ecs::Transform(0.0f, 0.0f));
    errorEntity.with(ecs::LuaScript("error.lua"));

    auto safeEntity = world->createEntity();
    safeEntity.with(ecs::Transform(0.0f, 0.0f));
    safeEntity.with(ecs::LuaScript("safe.lua"));

    luaSystem->update(registry, 0.016f);

    // Safe entity should have been processed
    EXPECT_FLOAT_EQ(safeEntity.get<ecs::Transform>().getPosition().x, 999.0f);
}

// ========== Frame Rate Independence Tests ==========

TEST_F(LuaSystemAdapterTest, ConsistentMovementAtDifferentFrameRates) {
    createTestScript("velocity_movement.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = transform.x + 100 * deltaTime
            end
        end
    )");

    auto &registry1 = world->getRegistry();
    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));
    entity.with(ecs::LuaScript("velocity_movement.lua"));

    // Simulate 1 second at 60 FPS
    for (int i = 0; i < 60; i++) {
        luaSystem->update(registry1, 1.0f / 60.0f);
    }

    auto &transform = entity.get<ecs::Transform>();
    EXPECT_NEAR(transform.getPosition().x, 100.0f, 0.1f);  // Should be ~100 after 1 second
}

TEST_F(LuaSystemAdapterTest, MultipleupdatesAccumulate) {
    createTestScript("accumulator.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = transform.x + 1
            end
        end
    )");

    auto &registry = world->getRegistry();
    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));
    entity.with(ecs::LuaScript("accumulator.lua"));

    for (int i = 0; i < 10; i++) {
        luaSystem->update(registry, 0.016f);
    }

    auto &transform = entity.get<ecs::Transform>();
    EXPECT_FLOAT_EQ(transform.getPosition().x, 10.0f);
}

// ========== Complex Behavior Tests ==========

TEST_F(LuaSystemAdapterTest, SineWaveMovement) {
    createTestScript("sine_wave.lua", R"(
        time = 0
        amplitude = 50
        frequency = 2
        
        function onUpdate(entity, deltaTime)
            time = time + deltaTime
            
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.y = amplitude * math.sin(time * frequency)
            end
        end
    )");

    auto &registry = world->getRegistry();
    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));
    entity.with(ecs::LuaScript("sine_wave.lua"));

    // Run for several frames
    for (int i = 0; i < 100; i++) {
        luaSystem->update(registry, 0.016f);
    }

    auto &transform = entity.get<ecs::Transform>();
    // Y should be oscillating, within amplitude range
    EXPECT_LE(std::abs(transform.getPosition().y), 50.0f);
}

TEST_F(LuaSystemAdapterTest, ConditionalBehavior) {
    createTestScript("boundary_bounce.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = transform.x + 10
                
                -- Bounce at boundary
                if transform.x > 100 then
                    transform.x = 0
                end
            end
        end
    )");

    auto &registry = world->getRegistry();
    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));
    entity.with(ecs::LuaScript("boundary_bounce.lua"));

    // Move past boundary
    for (int i = 0; i < 15; i++) {
        luaSystem->update(registry, 0.016f);
    }

    auto &transform = entity.get<ecs::Transform>();
    // After 15 updates (150 units), should have bounced back
    EXPECT_LT(transform.getPosition().x, 100.0f);
}
