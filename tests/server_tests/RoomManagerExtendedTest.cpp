/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomManagerExtendedTest.cpp - Extended tests for RoomManager to increase coverage
*/

#include <gtest/gtest.h>
#include <memory>
#include <thread>
#include "server/Rooms/Matchmaking/MatchmakingService.hpp"
#include "server/Rooms/RoomManager/RoomManager.hpp"

using namespace server;

class RoomManagerExtendedTest : public ::testing::Test {
   protected:
    std::shared_ptr<MatchmakingService> matchmaking;
    std::shared_ptr<RoomManager> roomManager;

    void SetUp() override {
        matchmaking = std::make_shared<MatchmakingService>(2, 4);
        roomManager = std::make_shared<RoomManager>(matchmaking);
    }
};

// ============================================================================
// Tests de création de room avec paramètres variés
// ============================================================================

TEST_F(RoomManagerExtendedTest, CreateRoomWithCustomName) {
    auto room = roomManager->createRoom("CustomRoom", "My Room", 8, false);
    EXPECT_NE(room, nullptr);
    EXPECT_EQ(room->getName(), "My Room");
    EXPECT_EQ(room->getId(), "CustomRoom");
}

TEST_F(RoomManagerExtendedTest, CreatePrivateRoom) {
    auto room = roomManager->createRoom("PrivateRoom", "Secret", 4, true);
    EXPECT_NE(room, nullptr);
    EXPECT_TRUE(room->isPrivate());
}

TEST_F(RoomManagerExtendedTest, CreatePublicRoom) {
    auto room = roomManager->createRoom("PublicRoom", "Open", 4, false);
    EXPECT_NE(room, nullptr);
    EXPECT_FALSE(room->isPrivate());
}

TEST_F(RoomManagerExtendedTest, CreateRoomWithDifferentSizes) {
    auto room2 = roomManager->createRoom("room2", "Room2", 2, false);
    auto room4 = roomManager->createRoom("room4", "Room4", 4, false);
    auto room8 = roomManager->createRoom("room8", "Room8", 8, false);
    auto room16 = roomManager->createRoom("room16", "Room16", 16, false);

    EXPECT_EQ(room2->getMaxPlayers(), 2);
    EXPECT_EQ(room4->getMaxPlayers(), 4);
    EXPECT_EQ(room8->getMaxPlayers(), 8);
    EXPECT_EQ(room16->getMaxPlayers(), 16);
}

TEST_F(RoomManagerExtendedTest, CreateManyRooms) {
    for (int i = 0; i < 20; ++i) {
        std::string id = "room_" + std::to_string(i);
        auto room = roomManager->createRoom(id);
        EXPECT_NE(room, nullptr);
    }

    EXPECT_EQ(roomManager->getRoomCount(), 20);
}

// ============================================================================
// Tests de récupération de rooms
// ============================================================================

TEST_F(RoomManagerExtendedTest, GetRoomById) {
    auto room = roomManager->createRoom("test-room");
    ASSERT_NE(room, nullptr);

    auto retrieved = roomManager->getRoom("test-room");
    EXPECT_EQ(retrieved, room);
}

TEST_F(RoomManagerExtendedTest, GetNonExistentRoom) {
    auto room = roomManager->getRoom("does-not-exist");
    EXPECT_EQ(room, nullptr);
}

TEST_F(RoomManagerExtendedTest, GetRoomAfterRemove) {
    roomManager->createRoom("temp-room");
    roomManager->removeRoom("temp-room");

    auto room = roomManager->getRoom("temp-room");
    EXPECT_EQ(room, nullptr);
}

// ============================================================================
// Tests de suppression de rooms
// ============================================================================

TEST_F(RoomManagerExtendedTest, RemoveExistingRoom) {
    roomManager->createRoom("to-remove");
    EXPECT_EQ(roomManager->getRoomCount(), 1);

    roomManager->removeRoom("to-remove");
    EXPECT_EQ(roomManager->getRoomCount(), 0);
}

TEST_F(RoomManagerExtendedTest, RemoveNonExistentRoom) {
    EXPECT_NO_THROW(roomManager->removeRoom("does-not-exist"));
}

TEST_F(RoomManagerExtendedTest, RemoveRoomTwice) {
    roomManager->createRoom("room");
    roomManager->removeRoom("room");
    EXPECT_NO_THROW(roomManager->removeRoom("room"));
}

TEST_F(RoomManagerExtendedTest, RemoveAllRooms) {
    roomManager->createRoom("room1");
    roomManager->createRoom("room2");
    roomManager->createRoom("room3");

    roomManager->removeRoom("room1");
    roomManager->removeRoom("room2");
    roomManager->removeRoom("room3");

    EXPECT_EQ(roomManager->getRoomCount(), 0);
}

// ============================================================================
// Tests de liste de rooms
// ============================================================================

TEST_F(RoomManagerExtendedTest, GetAllRooms) {
    roomManager->createRoom("room1");
    roomManager->createRoom("room2");
    roomManager->createRoom("room3");

    auto rooms = roomManager->getAllRooms();
    EXPECT_EQ(rooms.size(), 3);
}

TEST_F(RoomManagerExtendedTest, GetAllRoomsEmpty) {
    auto rooms = roomManager->getAllRooms();
    EXPECT_TRUE(rooms.empty());
}

TEST_F(RoomManagerExtendedTest, GetPublicRooms) {
    roomManager->createRoom("public1", "Public1", 4, false);
    roomManager->createRoom("private1", "Private1", 4, true);
    roomManager->createRoom("public2", "Public2", 4, false);

    auto publicRooms = roomManager->getPublicRooms();
    EXPECT_EQ(publicRooms.size(), 2);
}

TEST_F(RoomManagerExtendedTest, GetPublicRoomsOnlyPrivate) {
    roomManager->createRoom("private1", "Private1", 4, true);
    roomManager->createRoom("private2", "Private2", 4, true);

    auto publicRooms = roomManager->getPublicRooms();
    EXPECT_TRUE(publicRooms.empty());
}

TEST_F(RoomManagerExtendedTest, GetRooms) {
    roomManager->createRoom("room1");
    roomManager->createRoom("room2");

    auto roomList = roomManager->getAllRooms();
    EXPECT_GE(roomList.size(), 2);
}

// ============================================================================
// Tests de recherche de room par joueur
// ============================================================================

TEST_F(RoomManagerExtendedTest, GetRoomByPlayer) {
    auto room = roomManager->createRoom("player-room");
    ASSERT_NE(room, nullptr);

    room->join(100);

    auto foundRoom = roomManager->getRoomByPlayer(100);
    EXPECT_EQ(foundRoom, room);
}

TEST_F(RoomManagerExtendedTest, GetRoomByNonExistentPlayer) {
    roomManager->createRoom("room");

    auto room = roomManager->getRoomByPlayer(999);
    EXPECT_EQ(room, nullptr);
}

TEST_F(RoomManagerExtendedTest, GetRoomByPlayerInMultipleRooms) {
    auto room1 = roomManager->createRoom("room1");
    auto room2 = roomManager->createRoom("room2");

    room1->join(100);
    room2->join(200);

    auto found1 = roomManager->getRoomByPlayer(100);
    auto found2 = roomManager->getRoomByPlayer(200);

    EXPECT_EQ(found1, room1);
    EXPECT_EQ(found2, room2);
}

// ============================================================================
// Tests de comptage
// ============================================================================

TEST_F(RoomManagerExtendedTest, GetRoomCountZero) {
    EXPECT_EQ(roomManager->getRoomCount(), 0);
}

TEST_F(RoomManagerExtendedTest, GetRoomCountIncreases) {
    roomManager->createRoom("room1");
    EXPECT_EQ(roomManager->getRoomCount(), 1);

    roomManager->createRoom("room2");
    EXPECT_EQ(roomManager->getRoomCount(), 2);

    roomManager->createRoom("room3");
    EXPECT_EQ(roomManager->getRoomCount(), 3);
}

TEST_F(RoomManagerExtendedTest, GetRoomCountDecreases) {
    roomManager->createRoom("room1");
    roomManager->createRoom("room2");
    roomManager->createRoom("room3");

    roomManager->removeRoom("room2");
    EXPECT_EQ(roomManager->getRoomCount(), 2);
}

// ============================================================================
// Tests de mise à jour
// ============================================================================

TEST_F(RoomManagerExtendedTest, UpdateWithNoRooms) {
    EXPECT_NO_THROW(roomManager->update(1.0f / 60.0f));
}

TEST_F(RoomManagerExtendedTest, UpdateWithRooms) {
    roomManager->createRoom("room1");
    roomManager->createRoom("room2");

    EXPECT_NO_THROW(roomManager->update(1.0f / 60.0f));
}

TEST_F(RoomManagerExtendedTest, UpdateMultipleTimes) {
    auto room = roomManager->createRoom("room");
    ASSERT_NE(room, nullptr);

    for (int i = 0; i < 100; ++i) {
        EXPECT_NO_THROW(roomManager->update(1.0f / 60.0f));
    }
}

TEST_F(RoomManagerExtendedTest, UpdateWithZeroDelta) {
    auto room = roomManager->createRoom("room");
    EXPECT_NO_THROW(roomManager->update(0.0f));
}

TEST_F(RoomManagerExtendedTest, UpdateWithLargeDelta) {
    auto room = roomManager->createRoom("room");
    EXPECT_NO_THROW(roomManager->update(1.0f));
}

// ============================================================================
// Tests d'intégration avec matchmaking
// ============================================================================

TEST_F(RoomManagerExtendedTest, RoomManagerWithMatchmaking) {
    EXPECT_NE(roomManager, nullptr);
    // Le matchmaking service devrait être initialisé
}

TEST_F(RoomManagerExtendedTest, CreateRoomWithMatchmaking) {
    auto room = roomManager->createRoom("match-room");
    ASSERT_NE(room, nullptr);

    // Room créée avec succès même avec matchmaking actif
    EXPECT_EQ(room->getId(), "match-room");
}

// ============================================================================
// Tests de cycle de vie complet
// ============================================================================

TEST_F(RoomManagerExtendedTest, CompleteRoomLifecycle) {
    // Create
    auto room = roomManager->createRoom("lifecycle-room", "Test Room", 4, false);
    ASSERT_NE(room, nullptr);
    EXPECT_EQ(roomManager->getRoomCount(), 1);

    // Add players
    room->join(1);
    room->join(2);
    EXPECT_EQ(room->getPlayerCount(), 2);

    // Update
    roomManager->update(1.0f / 60.0f);

    // Remove
    roomManager->removeRoom("lifecycle-room");
    EXPECT_EQ(roomManager->getRoomCount(), 0);
}

TEST_F(RoomManagerExtendedTest, MultipleRoomsWithPlayers) {
    auto room1 = roomManager->createRoom("room1");
    auto room2 = roomManager->createRoom("room2");
    auto room3 = roomManager->createRoom("room3");

    room1->join(1);
    room1->join(2);
    room2->join(3);
    room3->join(4);
    room3->join(5);

    EXPECT_EQ(room1->getPlayerCount(), 2);
    EXPECT_EQ(room2->getPlayerCount(), 1);
    EXPECT_EQ(room3->getPlayerCount(), 2);

    auto found1 = roomManager->getRoomByPlayer(1);
    auto found3 = roomManager->getRoomByPlayer(3);
    auto found5 = roomManager->getRoomByPlayer(5);

    EXPECT_EQ(found1, room1);
    EXPECT_EQ(found3, room2);
    EXPECT_EQ(found5, room3);
}

TEST_F(RoomManagerExtendedTest, RoomStatesProgression) {
    auto room = roomManager->createRoom("state-room");
    ASSERT_NE(room, nullptr);

    EXPECT_EQ(room->getState(), RoomState::WAITING);

    room->join(1);
    room->join(2);
    room->setHost(1);

    // Update plusieurs fois
    for (int i = 0; i < 10; ++i) {
        roomManager->update(1.0f / 60.0f);
    }

    // Room devrait toujours exister
    EXPECT_NE(roomManager->getRoom("state-room"), nullptr);
}

TEST_F(RoomManagerExtendedTest, RemoveRoomWithPlayers) {
    auto room = roomManager->createRoom("room-with-players");
    ASSERT_NE(room, nullptr);

    room->join(1);
    room->join(2);
    room->join(3);

    // Devrait pouvoir supprimer même avec des joueurs
    EXPECT_NO_THROW(roomManager->removeRoom("room-with-players"));
    EXPECT_EQ(roomManager->getRoomCount(), 0);
}

TEST_F(RoomManagerExtendedTest, RoomManagerStress) {
    // Création rapide de nombreuses rooms
    for (int i = 0; i < 50; ++i) {
        std::string id = "stress_" + std::to_string(i);
        auto room = roomManager->createRoom(id);
        EXPECT_NE(room, nullptr);
    }

    EXPECT_EQ(roomManager->getRoomCount(), 50);

    // Update sur toutes les rooms
    for (int frame = 0; frame < 10; ++frame) {
        roomManager->update(1.0f / 60.0f);
    }

    // Suppression rapide
    for (int i = 0; i < 50; ++i) {
        std::string id = "stress_" + std::to_string(i);
        roomManager->removeRoom(id);
    }

    EXPECT_EQ(roomManager->getRoomCount(), 0);
}
