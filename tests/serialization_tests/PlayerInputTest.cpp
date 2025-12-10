/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 10/12/2025.
** File description:
** PlayerInputTest.cpp
*/

#include <gtest/gtest.h>
#include <vector>
#include "Capnp/Messages/C2S/PlayerInput.hpp"
#include "Capnp/Messages/Shared/SharedTypes.hpp"

TEST(PlayerInputTest, SerializeDeserializeRoundTrip) {
    RType::Messages::C2S::PlayerInput input;
    input._sequenceId = 42;
    input.actions = {RType::Messages::Shared::Action::MoveUp, RType::Messages::Shared::Action::Shoot};
    auto bytes = input.serialize();
    auto deserialized = RType::Messages::C2S::PlayerInput::deserialize(bytes);
    EXPECT_EQ(deserialized._sequenceId, 42);
    ASSERT_EQ(deserialized.actions.size(), 2);
    EXPECT_EQ(deserialized.actions[0], RType::Messages::Shared::Action::MoveUp);
    EXPECT_EQ(deserialized.actions[1], RType::Messages::Shared::Action::Shoot);
}

TEST(PlayerInputTest, EmptyActionList) {
    RType::Messages::C2S::PlayerInput input;
    input._sequenceId = 1;
    input.actions = {};
    auto bytes = input.serialize();
    auto deserialized = RType::Messages::C2S::PlayerInput::deserialize(bytes);
    EXPECT_EQ(deserialized._sequenceId, 1);
    EXPECT_TRUE(deserialized.actions.empty());
}

TEST(PlayerInputTest, VariousActionCombinations) {
    using Action = RType::Messages::Shared::Action;
    std::vector<std::vector<Action>> combos = {{Action::MoveUp},
                                               {Action::MoveDown, Action::MoveLeft},
                                               {Action::MoveRight, Action::Shoot, Action::MoveUp},
                                               {Action::Shoot, Action::Shoot, Action::Shoot}};
    uint32_t seq = 10;
    for (const auto &actions : combos) {
        RType::Messages::C2S::PlayerInput input;
        input._sequenceId = seq++;
        input.actions = actions;
        auto bytes = input.serialize();
        auto deserialized = RType::Messages::C2S::PlayerInput::deserialize(bytes);
        EXPECT_EQ(deserialized._sequenceId, seq - 1);
        ASSERT_EQ(deserialized.actions.size(), actions.size());
        for (size_t i = 0; i < actions.size(); ++i) {
            EXPECT_EQ(deserialized.actions[i], actions[i]);
        }
    }
}

TEST(PlayerInputTest, SequenceIdEdgeCases) {
    using PI = RType::Messages::C2S::PlayerInput;
    std::vector<uint32_t> ids = {0, 1, UINT32_MAX, UINT32_MAX - 1};
    for (auto id : ids) {
        PI input;
        input._sequenceId = id;
        input.actions = {RType::Messages::Shared::Action::MoveLeft};
        auto bytes = input.serialize();
        auto deserialized = PI::deserialize(bytes);
        EXPECT_EQ(deserialized._sequenceId, id);
        ASSERT_EQ(deserialized.actions.size(), 1);
        EXPECT_EQ(deserialized.actions[0], RType::Messages::Shared::Action::MoveLeft);
    }
}
