/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 10/12/2025.
** File description:
** GameStartTest.cpp
*/

#include <gtest/gtest.h>
#include <vector>
#include "Capnp/Messages/S2C/EntityState.hpp"
#include "Capnp/Messages/S2C/GameStart.hpp"
#include "Capnp/Messages/Shared/SharedTypes.hpp"

TEST(GameStartTest, SerializeDeserializeRoundTrip) {
    RType::Messages::S2C::GameStart start;
    start.yourEntityId = 42;
    start.initialState.serverTick = 123;
    RType::Messages::S2C::EntityState entity;
    entity.entityId = 1001;
    entity.type = RType::Messages::Shared::EntityType::Player;
    entity.position = RType::Messages::Shared::Vec2(10.0F, 20.0F);
    entity.health = 100;
    start.initialState.entities.push_back(entity);

    auto bytes = start.serialize();
    auto deserialized = RType::Messages::S2C::GameStart::deserialize(bytes);

    EXPECT_EQ(deserialized.yourEntityId, 42);
    EXPECT_EQ(deserialized.initialState.serverTick, 123);
    ASSERT_EQ(deserialized.initialState.entities.size(), 1);
    EXPECT_EQ(deserialized.initialState.entities[0].entityId, 1001);
    EXPECT_EQ(deserialized.initialState.entities[0].type, RType::Messages::Shared::EntityType::Player);
    EXPECT_FLOAT_EQ(deserialized.initialState.entities[0].position.x, 10.0F);
    EXPECT_FLOAT_EQ(deserialized.initialState.entities[0].position.y, 20.0F);
    EXPECT_EQ(deserialized.initialState.entities[0].health, 100);
}

TEST(GameStartTest, EmptyEntityList) {
    RType::Messages::S2C::GameStart start;
    start.yourEntityId = 1;
    start.initialState.serverTick = 0;
    // No entities
    auto bytes = start.serialize();
    auto deserialized = RType::Messages::S2C::GameStart::deserialize(bytes);
    EXPECT_EQ(deserialized.yourEntityId, 1);
    EXPECT_EQ(deserialized.initialState.serverTick, 0);
    EXPECT_TRUE(deserialized.initialState.entities.empty());
}

TEST(GameStartTest, VariousEntityTypes) {
    RType::Messages::S2C::GameStart start;
    start.yourEntityId = 99;
    start.initialState.serverTick = 555;
    // Player
    RType::Messages::S2C::EntityState player;
    player.entityId = 1001;
    player.type = RType::Messages::Shared::EntityType::Player;
    player.position = RType::Messages::Shared::Vec2(1.0F, 2.0F);
    player.health = 100;
    start.initialState.entities.push_back(player);
    // Enemy
    RType::Messages::S2C::EntityState enemy;
    enemy.entityId = 2001;
    enemy.type = RType::Messages::Shared::EntityType::EnemyType1;
    enemy.position = RType::Messages::Shared::Vec2(3.0F, 4.0F);
    enemy.health = 50;
    start.initialState.entities.push_back(enemy);
    // Bullet (no health)
    RType::Messages::S2C::EntityState bullet;
    bullet.entityId = 3001;
    bullet.type = RType::Messages::Shared::EntityType::PlayerBullet;
    bullet.position = RType::Messages::Shared::Vec2(5.0F, 6.0F);
    bullet.health = -1;
    start.initialState.entities.push_back(bullet);

    auto bytes = start.serialize();
    auto deserialized = RType::Messages::S2C::GameStart::deserialize(bytes);
    EXPECT_EQ(deserialized.yourEntityId, 99);
    EXPECT_EQ(deserialized.initialState.serverTick, 555);
    ASSERT_EQ(deserialized.initialState.entities.size(), 3);
    EXPECT_EQ(deserialized.initialState.entities[0].type, RType::Messages::Shared::EntityType::Player);
    EXPECT_EQ(deserialized.initialState.entities[1].type, RType::Messages::Shared::EntityType::EnemyType1);
    EXPECT_EQ(deserialized.initialState.entities[2].type, RType::Messages::Shared::EntityType::PlayerBullet);
    EXPECT_EQ(deserialized.initialState.entities[2].health, std::nullopt);
}

TEST(GameStartTest, DeserializeCorruptData) {
    std::vector<uint8_t> badData = {0x01, 0x02, 0x03};
    // Should throw or fail gracefully
    try {
        auto result = RType::Messages::S2C::GameStart::deserialize(badData);
        FAIL() << "Expected exception or error on corrupt data";
    } catch (const std::exception &e) {
        SUCCEED();
    } catch (...) {
        SUCCEED();
    }
}
