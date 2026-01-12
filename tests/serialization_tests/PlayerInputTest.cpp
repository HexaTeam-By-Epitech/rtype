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
    RType::Messages::C2S::PlayerInput::InputSnapshot snapshot;
    snapshot.sequenceId = 42;
    snapshot.actions = {RType::Messages::Shared::Action::MoveUp, RType::Messages::Shared::Action::Shoot};
    input.inputs.push_back(snapshot);

    auto bytes = input.serialize();
    auto deserialized = RType::Messages::C2S::PlayerInput::deserialize(bytes);

    ASSERT_EQ(deserialized.inputs.size(), 1);
    EXPECT_EQ(deserialized.inputs[0].sequenceId, 42);
    ASSERT_EQ(deserialized.inputs[0].actions.size(), 2);
    EXPECT_EQ(deserialized.inputs[0].actions[0], RType::Messages::Shared::Action::MoveUp);
    EXPECT_EQ(deserialized.inputs[0].actions[1], RType::Messages::Shared::Action::Shoot);
}

TEST(PlayerInputTest, EmptyActionList) {
    RType::Messages::C2S::PlayerInput input;
    RType::Messages::C2S::PlayerInput::InputSnapshot snapshot;
    snapshot.sequenceId = 1;
    snapshot.actions = {};
    input.inputs.push_back(snapshot);

    auto bytes = input.serialize();
    auto deserialized = RType::Messages::C2S::PlayerInput::deserialize(bytes);

    ASSERT_EQ(deserialized.inputs.size(), 1);
    EXPECT_EQ(deserialized.inputs[0].sequenceId, 1);
    EXPECT_TRUE(deserialized.inputs[0].actions.empty());
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
        RType::Messages::C2S::PlayerInput::InputSnapshot snapshot;
        snapshot.sequenceId = seq++;
        snapshot.actions = actions;
        input.inputs.push_back(snapshot);

        auto bytes = input.serialize();
        auto deserialized = RType::Messages::C2S::PlayerInput::deserialize(bytes);

        ASSERT_EQ(deserialized.inputs.size(), 1);
        EXPECT_EQ(deserialized.inputs[0].sequenceId, seq - 1);
        ASSERT_EQ(deserialized.inputs[0].actions.size(), actions.size());
        for (size_t i = 0; i < actions.size(); ++i) {
            EXPECT_EQ(deserialized.inputs[0].actions[i], actions[i]);
        }
    }
}

TEST(PlayerInputTest, SequenceIdEdgeCases) {
    using PI = RType::Messages::C2S::PlayerInput;
    std::vector<uint32_t> ids = {0, 1, UINT32_MAX, UINT32_MAX - 1};
    for (auto id : ids) {
        PI input;
        PI::InputSnapshot snapshot;
        snapshot.sequenceId = id;
        snapshot.actions = {RType::Messages::Shared::Action::MoveLeft};
        input.inputs.push_back(snapshot);

        auto bytes = input.serialize();
        auto deserialized = PI::deserialize(bytes);

        ASSERT_EQ(deserialized.inputs.size(), 1);
        EXPECT_EQ(deserialized.inputs[0].sequenceId, id);
        ASSERT_EQ(deserialized.inputs[0].actions.size(), 1);
        EXPECT_EQ(deserialized.inputs[0].actions[0], RType::Messages::Shared::Action::MoveLeft);
    }
}

TEST(PlayerInputTest, HistoryRedundancy) {
    // Test multiple inputs in one packet
    RType::Messages::C2S::PlayerInput input;

    // Add 3 snapshots
    for (int i = 0; i < 3; ++i) {
        RType::Messages::C2S::PlayerInput::InputSnapshot snapshot;
        snapshot.sequenceId = 100 + i;
        snapshot.actions = {RType::Messages::Shared::Action::MoveUp};
        input.inputs.push_back(snapshot);
    }

    auto bytes = input.serialize();
    auto deserialized = RType::Messages::C2S::PlayerInput::deserialize(bytes);

    ASSERT_EQ(deserialized.inputs.size(), 3);
    EXPECT_EQ(deserialized.inputs[0].sequenceId, 100);
    EXPECT_EQ(deserialized.inputs[1].sequenceId, 101);
    EXPECT_EQ(deserialized.inputs[2].sequenceId, 102);
}