/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 10/12/2025.
** File description:
** GameStateTest.cpp
*/

#include <gtest/gtest.h>
#include <vector>
#include "Capnp/Messages/S2C/EntityState.hpp"
#include "Capnp/Messages/S2C/GameState.hpp"
#include "Capnp/Messages/Shared/SharedTypes.hpp"

TEST(GameStateTest, SerializeDeserializeRoundTrip) {
    RType::Messages::S2C::GameState state;
    state.serverTick = 42;
    RType::Messages::S2C::EntityState entity;
    entity.entityId = 1234;
    entity.type = RType::Messages::Shared::EntityType::Player;
    entity.position = RType::Messages::Shared::Vec2(100.0F, 200.0F);
    entity.health = 99;
    state.entities.push_back(entity);

    auto bytes = state.serialize();
    auto deserialized = RType::Messages::S2C::GameState::deserialize(bytes);

    EXPECT_EQ(deserialized.serverTick, 42);
    ASSERT_EQ(deserialized.entities.size(), 1);
    EXPECT_EQ(deserialized.entities[0].entityId, 1234);
    EXPECT_EQ(deserialized.entities[0].type, RType::Messages::Shared::EntityType::Player);
    EXPECT_FLOAT_EQ(deserialized.entities[0].position.x, 100.0F);
    EXPECT_FLOAT_EQ(deserialized.entities[0].position.y, 200.0F);
    EXPECT_EQ(deserialized.entities[0].health, 99);
}

TEST(GameStateTest, LargeEntityList) {
    RType::Messages::S2C::GameState state;
    state.serverTick = 100;
    const int numEntities = 1000;
    for (int i = 0; i < numEntities; ++i) {
        RType::Messages::S2C::EntityState entity;
        entity.entityId = 2000 + i;
        entity.type = RType::Messages::Shared::EntityType::EnemyType1;
        entity.position = RType::Messages::Shared::Vec2(i * 1.0F, i * 2.0F);
        entity.health = i % 100;
        state.entities.push_back(entity);
    }
    auto bytes = state.serialize();
    auto deserialized = RType::Messages::S2C::GameState::deserialize(bytes);
    EXPECT_EQ(deserialized.serverTick, 100);
    ASSERT_EQ(deserialized.entities.size(), numEntities);
    for (int i = 0; i < numEntities; ++i) {
        EXPECT_EQ(deserialized.entities[i].entityId, 2000 + i);
        EXPECT_EQ(deserialized.entities[i].type, RType::Messages::Shared::EntityType::EnemyType1);
        EXPECT_FLOAT_EQ(deserialized.entities[i].position.x, i * 1.0F);
        EXPECT_FLOAT_EQ(deserialized.entities[i].position.y, i * 2.0F);
        EXPECT_EQ(deserialized.entities[i].health, i % 100);
    }
}

TEST(GameStateTest, VariousEntityStates) {
    RType::Messages::S2C::GameState state;
    state.serverTick = 7;
    RType::Messages::S2C::EntityState player;
    player.entityId = 1001;
    player.type = RType::Messages::Shared::EntityType::Player;
    player.position = RType::Messages::Shared::Vec2(10.0F, 20.0F);
    player.health = 100;
    state.entities.push_back(player);

    RType::Messages::S2C::EntityState enemy;
    enemy.entityId = 2001;
    enemy.type = RType::Messages::Shared::EntityType::EnemyType1;
    enemy.position = RType::Messages::Shared::Vec2(30.0F, 40.0F);
    enemy.health = 50;
    state.entities.push_back(enemy);

    RType::Messages::S2C::EntityState bullet;
    bullet.entityId = 3001;
    bullet.type = RType::Messages::Shared::EntityType::PlayerBullet;
    bullet.position = RType::Messages::Shared::Vec2(50.0F, 60.0F);
    bullet.health = -1;  // No health
    state.entities.push_back(bullet);

    auto bytes = state.serialize();
    auto deserialized = RType::Messages::S2C::GameState::deserialize(bytes);
    EXPECT_EQ(deserialized.serverTick, 7);
    ASSERT_EQ(deserialized.entities.size(), 3);
    EXPECT_EQ(deserialized.entities[0].type, RType::Messages::Shared::EntityType::Player);
    EXPECT_EQ(deserialized.entities[1].type, RType::Messages::Shared::EntityType::EnemyType1);
    EXPECT_EQ(deserialized.entities[2].type, RType::Messages::Shared::EntityType::PlayerBullet);
    EXPECT_EQ(deserialized.entities[2].health, std::nullopt);
}
