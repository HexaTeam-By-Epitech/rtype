/*
** EPITECH PROJECT, 2026
** rtype
** File description:
** LuaEngineTest - Tests for Lua scripting system
*/

#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>
#include "common/ECS/Components/Health.hpp"
#include "common/ECS/Components/LuaScript.hpp"
#include "common/ECS/Components/Transform.hpp"
#include "common/ECS/Components/Velocity.hpp"
#include "common/ECSWrapper/ECSWorld.hpp"
#include "server/Scripting/LuaEngine.hpp"

class LuaEngineTest : public ::testing::Test {
   protected:
    void SetUp() override {
        world = std::make_unique<ecs::wrapper::ECSWorld>();
        luaEngine = std::make_unique<scripting::LuaEngine>("./test_scripts/");
        luaEngine->setWorld(world.get());

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
};

// ========== LuaEngine Initialization Tests ==========

TEST_F(LuaEngineTest, EngineInitializesWithWorld) {
    EXPECT_NO_THROW(luaEngine->setWorld(world.get()));
}

TEST_F(LuaEngineTest, LoadNonExistentScriptFails) {
    EXPECT_FALSE(luaEngine->loadScript("non_existent_script.lua"));
}

TEST_F(LuaEngineTest, LoadValidScriptSucceeds) {
    createTestScript("simple.lua", R"(
        function onUpdate(entity, deltaTime)
        end
    )");

    EXPECT_TRUE(luaEngine->loadScript("simple.lua"));
}

TEST_F(LuaEngineTest, LoadScriptWithSyntaxErrorFails) {
    createTestScript("syntax_error.lua", R"(
        function onUpdate(entity, deltaTime
            -- Missing closing parenthesis
        end
    )");

    EXPECT_FALSE(luaEngine->loadScript("syntax_error.lua"));
}

// ========== Component Binding Tests ==========

TEST_F(LuaEngineTest, TransformComponentAccessible) {
    createTestScript("test_transform.lua", R"(
        testPassed = false
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                if transform.x == 100.0 and transform.y == 200.0 then
                    testPassed = true
                end
            end
        end
    )");

    auto entity = world->createEntity();
    entity.with(ecs::Transform(100.0f, 200.0f));

    luaEngine->loadScript("test_transform.lua");
    luaEngine->executeUpdate("test_transform.lua", entity, 0.016f);

    sol::state_view lua(luaEngine->getLuaState());
    bool testPassed = lua["testPassed"];
    EXPECT_TRUE(testPassed);
}

TEST_F(LuaEngineTest, TransformComponentModifiable) {
    createTestScript("modify_transform.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = 50.0
                transform.y = 75.0
            end
        end
    )");

    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));

    luaEngine->loadScript("modify_transform.lua");
    luaEngine->executeUpdate("modify_transform.lua", entity, 0.016f);

    auto &transform = entity.get<ecs::Transform>();
    EXPECT_FLOAT_EQ(transform.getPosition().x, 50.0f);
    EXPECT_FLOAT_EQ(transform.getPosition().y, 75.0f);
}

TEST_F(LuaEngineTest, VelocityComponentAccessible) {
    createTestScript("test_velocity.lua", R"(
        testPassed = false
        function onUpdate(entity, deltaTime)
            if entity:hasVelocity() then
                local velocity = entity:getVelocity()
                if velocity.speed == 100.0 then
                    testPassed = true
                end
            end
        end
    )");

    auto entity = world->createEntity();
    entity.with(ecs::Velocity(1.0f, 0.0f, 100.0f));

    luaEngine->loadScript("test_velocity.lua");
    luaEngine->executeUpdate("test_velocity.lua", entity, 0.016f);

    sol::state_view lua(luaEngine->getLuaState());
    bool testPassed = lua["testPassed"];
    EXPECT_TRUE(testPassed);
}

TEST_F(LuaEngineTest, HealthComponentModifiable) {
    createTestScript("damage.lua", R"(
        function onUpdate(entity, deltaTime)
            if entity:hasHealth() then
                local health = entity:getHealth()
                health.currentHealth = health.currentHealth - 10
            end
        end
    )");

    auto entity = world->createEntity();
    entity.with(ecs::Health(100));

    luaEngine->loadScript("damage.lua");
    luaEngine->executeUpdate("damage.lua", entity, 0.016f);

    auto &health = entity.get<ecs::Health>();
    EXPECT_EQ(health.getCurrentHealth(), 90);
}

// ========== Entity Validation Tests ==========

TEST_F(LuaEngineTest, ValidEntityDetected) {
    createTestScript("check_valid2.lua", R"(
        isValid = nil
        function onUpdate(entity, deltaTime)
            isValid = entity:isValid()
        end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("check_valid2.lua");
    luaEngine->executeUpdate("check_valid2.lua", entity, 0.016f);

    sol::state_view lua(luaEngine->getLuaState());
    bool isValid = lua["isValid"];
    EXPECT_TRUE(isValid);
}

// ========== Global Function Tests ==========

TEST_F(LuaEngineTest, LogFunctionWorks) {
    createTestScript("test_log.lua", R"(
        function onUpdate(entity, deltaTime)
            log("Test message from Lua")
        end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("test_log.lua");
    // Should not throw
    EXPECT_NO_THROW(luaEngine->executeUpdate("test_log.lua", entity, 0.016f));
}

TEST_F(LuaEngineTest, CreateEntityFromLua) {
    createTestScript("create_entity.lua", R"(
        newEntityValid = false
        function onUpdate(entity, deltaTime)
            local newEntity = createEntity()
            newEntityValid = newEntity:isValid()
        end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("create_entity.lua");
    luaEngine->executeUpdate("create_entity.lua", entity, 0.016f);

    sol::state_view lua(luaEngine->getLuaState());
    bool newEntityValid = lua["newEntityValid"];
    EXPECT_TRUE(newEntityValid);
}

// ========== Script Execution Tests ==========

TEST_F(LuaEngineTest, DeltaTimePassedCorrectly) {
    createTestScript("check_deltatime.lua", R"(
        receivedDeltaTime = 0
        function onUpdate(entity, deltaTime)
            receivedDeltaTime = deltaTime
        end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("check_deltatime.lua");
    luaEngine->executeUpdate("check_deltatime.lua", entity, 0.123f);

    sol::state_view lua(luaEngine->getLuaState());
    float receivedDeltaTime = lua["receivedDeltaTime"];
    EXPECT_FLOAT_EQ(receivedDeltaTime, 0.123f);
}

TEST_F(LuaEngineTest, ScriptExecutedMultipleTimes) {
    createTestScript("counter.lua", R"(
        counter = 0
        function onUpdate(entity, deltaTime)
            counter = counter + 1
        end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("counter.lua");

    for (int i = 0; i < 10; i++) {
        luaEngine->executeUpdate("counter.lua", entity, 0.016f);
    }

    sol::state_view lua(luaEngine->getLuaState());
    int counter = lua["counter"];
    EXPECT_EQ(counter, 10);
}

TEST_F(LuaEngineTest, CircularMovementPattern) {
    createTestScript("circular.lua", R"(
        time = 0
        centerX = 400
        centerY = 300
        radius = 150
        speed = 2.0
        
        function onUpdate(entity, deltaTime)
            time = time + deltaTime
            
            if entity:hasTransform() then
                local transform = entity:getTransform()
                transform.x = centerX + math.cos(time * speed) * radius
                transform.y = centerY + math.sin(time * speed) * radius
            end
        end
    )");

    auto entity = world->createEntity();
    entity.with(ecs::Transform(0.0f, 0.0f));

    luaEngine->loadScript("circular.lua");

    // Execute a few frames
    for (int i = 0; i < 60; i++) {
        luaEngine->executeUpdate("circular.lua", entity, 0.016f);
    }

    auto &transform = entity.get<ecs::Transform>();
    auto pos = transform.getPosition();

    // After ~1 second, position should have changed significantly from origin
    EXPECT_NE(pos.x, 0.0f);
    EXPECT_NE(pos.y, 0.0f);

    // Position should be within circular bounds
    float distFromCenter = std::sqrt(std::pow(pos.x - 400, 2) + std::pow(pos.y - 300, 2));
    EXPECT_NEAR(distFromCenter, 150.0f, 1.0f);
}

// ========== Math Helper Function Tests ==========

TEST_F(LuaEngineTest, RandomFunctionWorks) {
    createTestScript("test_random.lua", R"(
        randomValue = 0
        function onUpdate(entity, deltaTime)
            randomValue = random(0, 100)
        end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("test_random.lua");
    luaEngine->executeUpdate("test_random.lua", entity, 0.016f);

    sol::state_view lua(luaEngine->getLuaState());
    float randomValue = lua["randomValue"];

    EXPECT_GE(randomValue, 0.0f);
    EXPECT_LE(randomValue, 100.0f);
}

TEST_F(LuaEngineTest, DistanceFunctionWorks) {
    createTestScript("test_distance.lua", R"(
        calculatedDistance = 0
        function onUpdate(entity, deltaTime)
            calculatedDistance = distance(0, 0, 3, 4)
        end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("test_distance.lua");
    luaEngine->executeUpdate("test_distance.lua", entity, 0.016f);

    sol::state_view lua(luaEngine->getLuaState());
    float calculatedDistance = lua["calculatedDistance"];

    EXPECT_FLOAT_EQ(calculatedDistance, 5.0f);  // 3-4-5 triangle
}

// ========== Error Handling Tests ==========

TEST_F(LuaEngineTest, MissingOnUpdateFunctionLogged) {
    createTestScript("no_onupdate.lua", R"(
        -- No onUpdate function defined
        someOtherFunction = function() end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("no_onupdate.lua");
    // Should not crash, just log warning
    EXPECT_NO_THROW(luaEngine->executeUpdate("no_onupdate.lua", entity, 0.016f));
}

TEST_F(LuaEngineTest, RuntimeErrorInScriptCaught) {
    createTestScript("runtime_error.lua", R"(
        function onUpdate(entity, deltaTime)
            error("Intentional error for testing")
        end
    )");

    auto entity = world->createEntity();

    luaEngine->loadScript("runtime_error.lua");
    // Should catch the error and not crash
    EXPECT_NO_THROW(luaEngine->executeUpdate("runtime_error.lua", entity, 0.016f));
}

TEST_F(LuaEngineTest, AccessNonExistentComponentSafe) {
    createTestScript("access_missing.lua", R"(
        hasComponent = false
        function onUpdate(entity, deltaTime)
            hasComponent = entity:hasTransform()
        end
    )");

    auto entity = world->createEntity();
    // Don't add Transform component

    luaEngine->loadScript("access_missing.lua");
    luaEngine->executeUpdate("access_missing.lua", entity, 0.016f);

    sol::state_view lua(luaEngine->getLuaState());
    bool hasComponent = lua["hasComponent"];
    EXPECT_FALSE(hasComponent);
}
