/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RoomTest.cpp - Unit tests for Room, RoomManager, and Lobby
*/

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <thread>
#include "server/Rooms/IRoom.hpp"
#include "server/Rooms/Lobby/Lobby.hpp"
#include "server/Rooms/Room.hpp"
#include "server/Rooms/RoomManager/RoomManager.hpp"

using namespace server;

// ============================================================================
// Room Tests
// ============================================================================

class RoomTest : public ::testing::Test {
   protected:
    void SetUp() override { room = std::make_shared<Room>("test-room-1", "Test Room", 4, false); }

    void TearDown() override { room.reset(); }

    std::shared_ptr<Room> room;
};

TEST_F(RoomTest, Construction) {
    EXPECT_EQ(room->getId(), "test-room-1");
    EXPECT_EQ(room->getName(), "Test Room");
    EXPECT_EQ(room->getMaxPlayers(), 4);
    EXPECT_EQ(room->getState(), RoomState::WAITING);
    EXPECT_FALSE(room->isFull());
    EXPECT_EQ(room->getPlayerCount(), 0);
}

TEST_F(RoomTest, ConstructionWithDefaultName) {
    auto room2 = std::make_shared<Room>("room-2");
    EXPECT_EQ(room2->getId(), "room-2");
    EXPECT_EQ(room2->getName(), "room-2");  // Default name = id
}

TEST_F(RoomTest, JoinPlayer) {
    EXPECT_TRUE(room->join(1));
    EXPECT_EQ(room->getPlayerCount(), 1);
    EXPECT_TRUE(room->hasPlayer(1));

    EXPECT_TRUE(room->join(2));
    EXPECT_EQ(room->getPlayerCount(), 2);
}

TEST_F(RoomTest, JoinSamePlayerTwice) {
    EXPECT_TRUE(room->join(1));
    EXPECT_FALSE(room->join(1));  // Cannot join twice
    EXPECT_EQ(room->getPlayerCount(), 1);
}

TEST_F(RoomTest, JoinFullRoom) {
    EXPECT_TRUE(room->join(1));
    EXPECT_TRUE(room->join(2));
    EXPECT_TRUE(room->join(3));
    EXPECT_TRUE(room->join(4));
    EXPECT_TRUE(room->isFull());

    EXPECT_FALSE(room->join(5));  // Room is full
    EXPECT_EQ(room->getPlayerCount(), 4);
}

TEST_F(RoomTest, LeavePlayer) {
    room->join(1);
    room->join(2);

    EXPECT_TRUE(room->leave(1));
    EXPECT_EQ(room->getPlayerCount(), 1);
    EXPECT_FALSE(room->hasPlayer(1));
    EXPECT_TRUE(room->hasPlayer(2));
}

TEST_F(RoomTest, LeaveNonExistentPlayer) {
    room->join(1);
    EXPECT_FALSE(room->leave(99));  // Player not in room
    EXPECT_EQ(room->getPlayerCount(), 1);
}

TEST_F(RoomTest, GetPlayers) {
    room->join(1);
    room->join(2);
    room->join(3);

    auto players = room->getPlayers();
    EXPECT_EQ(players.size(), 3);
    EXPECT_TRUE(std::find(players.begin(), players.end(), 1) != players.end());
    EXPECT_TRUE(std::find(players.begin(), players.end(), 2) != players.end());
    EXPECT_TRUE(std::find(players.begin(), players.end(), 3) != players.end());
}

TEST_F(RoomTest, SetState) {
    EXPECT_EQ(room->getState(), RoomState::WAITING);

    room->setState(RoomState::STARTING);
    EXPECT_EQ(room->getState(), RoomState::STARTING);

    room->setState(RoomState::IN_PROGRESS);
    EXPECT_EQ(room->getState(), RoomState::IN_PROGRESS);

    room->setState(RoomState::FINISHED);
    EXPECT_EQ(room->getState(), RoomState::FINISHED);
}

TEST_F(RoomTest, GetRoomInfo) {
    room->join(1);
    room->join(2);
    room->setState(RoomState::STARTING);

    RoomInfo info = room->getInfo();
    EXPECT_EQ(info.id, "test-room-1");
    EXPECT_EQ(info.name, "Test Room");
    EXPECT_EQ(info.currentPlayers, 2);
    EXPECT_EQ(info.maxPlayers, 4);
    EXPECT_EQ(info.state, RoomState::STARTING);
}

TEST_F(RoomTest, SetAndGetHost) {
    EXPECT_EQ(room->getHost(), 0);  // No host initially

    room->join(1);
    room->setHost(1);
    EXPECT_EQ(room->getHost(), 1);

    room->join(2);
    room->setHost(2);
    EXPECT_EQ(room->getHost(), 2);
}

TEST_F(RoomTest, StartGame) {
    room->join(1);
    room->join(2);
    room->setHost(1);

    // Note: startGame requires game loop which is initialized in constructor
    // This is an integration point with ServerLoop
    EXPECT_NO_THROW(room->startGame());
}

TEST_F(RoomTest, RequestStartGame) {
    room->join(1);
    room->setHost(1);

    EXPECT_NO_THROW(room->requestStartGame());
    // State should transition to STARTING
    // Actual state change depends on timing
}

TEST_F(RoomTest, GetGameLogic) {
    auto gameLogic = room->getGameLogic();
    EXPECT_NE(gameLogic, nullptr);
}

TEST_F(RoomTest, GetServerLoop) {
    auto serverLoop = room->getServerLoop();
    EXPECT_NE(serverLoop, nullptr);
}

TEST_F(RoomTest, PrivateRoom) {
    auto privateRoom = std::make_shared<Room>("private-1", "Private Room", 2, true);
    auto info = privateRoom->getInfo();
    EXPECT_TRUE(info.isPrivate);
}

// ============================================================================
// RoomManager Tests
// ============================================================================

class RoomManagerTest : public ::testing::Test {
   protected:
    void SetUp() override { manager = std::make_shared<RoomManager>(); }

    void TearDown() override { manager.reset(); }

    std::shared_ptr<RoomManager> manager;
};

TEST_F(RoomManagerTest, CreateRoom) {
    auto room = manager->createRoom("room-1", "Test Room", 4, false);
    EXPECT_NE(room, nullptr);
    EXPECT_EQ(room->getId(), "room-1");
    EXPECT_EQ(room->getName(), "Test Room");
}

TEST_F(RoomManagerTest, CreateRoomWithDefaultParams) {
    auto room = manager->createRoom("room-2");
    EXPECT_NE(room, nullptr);
    EXPECT_EQ(room->getId(), "room-2");
}

TEST_F(RoomManagerTest, CreateDuplicateRoom) {
    auto room1 = manager->createRoom("duplicate");
    EXPECT_NE(room1, nullptr);

    auto room2 = manager->createRoom("duplicate");
    // Creating a duplicate room should fail gracefully (return nullptr or same room)
    // Based on logs, it returns nullptr
    EXPECT_TRUE(room2 == nullptr || room2 == room1);
}

TEST_F(RoomManagerTest, GetRoom) {
    manager->createRoom("room-1");

    auto room = manager->getRoom("room-1");
    EXPECT_NE(room, nullptr);
    EXPECT_EQ(room->getId(), "room-1");
}

TEST_F(RoomManagerTest, GetNonExistentRoom) {
    auto room = manager->getRoom("non-existent");
    EXPECT_EQ(room, nullptr);
}

TEST_F(RoomManagerTest, RemoveRoom) {
    manager->createRoom("room-to-remove");
    EXPECT_NE(manager->getRoom("room-to-remove"), nullptr);

    EXPECT_TRUE(manager->removeRoom("room-to-remove"));
    EXPECT_EQ(manager->getRoom("room-to-remove"), nullptr);
}

TEST_F(RoomManagerTest, RemoveNonExistentRoom) {
    EXPECT_FALSE(manager->removeRoom("non-existent"));
}

TEST_F(RoomManagerTest, GetAllRooms) {
    manager->createRoom("room-1");
    manager->createRoom("room-2");
    manager->createRoom("room-3");

    auto rooms = manager->getAllRooms();
    EXPECT_EQ(rooms.size(), 3);
}

TEST_F(RoomManagerTest, GetRooms) {
    manager->createRoom("room-1");
    manager->createRoom("room-2");
    manager->createRoom("room-3");

    auto rooms = manager->getAllRooms();
    EXPECT_EQ(rooms.size(), 3);
}

TEST_F(RoomManagerTest, GetRoomCount) {
    EXPECT_EQ(manager->getRoomCount(), 0);

    manager->createRoom("room-1");
    EXPECT_EQ(manager->getRoomCount(), 1);

    manager->createRoom("room-2");
    EXPECT_EQ(manager->getRoomCount(), 2);

    manager->removeRoom("room-1");
    EXPECT_EQ(manager->getRoomCount(), 1);
}

// ============================================================================
// Lobby Tests
// ============================================================================

class LobbyTest : public ::testing::Test {
   protected:
    void SetUp() override {
        roomManager = std::make_shared<RoomManager>();
        lobby = std::make_shared<Lobby>(roomManager);
    }

    void TearDown() override {
        lobby.reset();
        roomManager.reset();
    }

    std::shared_ptr<RoomManager> roomManager;
    std::shared_ptr<Lobby> lobby;
};

TEST_F(LobbyTest, Construction) {
    EXPECT_NE(lobby, nullptr);
}

TEST_F(LobbyTest, CreateRoom) {
    lobby->addPlayer(1, "Player1");
    std::string roomId = lobby->createCustomRoom(1, "Test Room", 4, false);
    EXPECT_FALSE(roomId.empty());

    auto room = roomManager->getRoom(roomId);
    EXPECT_NE(room, nullptr);
    EXPECT_EQ(room->getName(), "Test Room");
    EXPECT_EQ(room->getMaxPlayers(), 4);
}

TEST_F(LobbyTest, CreateMultipleRooms) {
    lobby->addPlayer(1, "Player1");
    lobby->addPlayer(2, "Player2");
    lobby->addPlayer(3, "Player3");

    std::string room1 = lobby->createCustomRoom(1, "Room 1", 2, false);
    std::string room2 = lobby->createCustomRoom(2, "Room 2", 4, false);
    std::string room3 = lobby->createCustomRoom(3, "Room 3", 8, true);

    EXPECT_FALSE(room1.empty());
    EXPECT_FALSE(room2.empty());
    EXPECT_FALSE(room3.empty());
    EXPECT_NE(room1, room2);
    EXPECT_NE(room2, room3);
}

TEST_F(LobbyTest, GetRooms) {
    lobby->addPlayer(1, "Player1");
    lobby->addPlayer(2, "Player2");

    lobby->createCustomRoom(1, "Room 1", 4, false);
    lobby->createCustomRoom(2, "Room 2", 4, false);

    auto rooms = roomManager->getAllRooms();
    EXPECT_GE(rooms.size(), 2);
}

TEST_F(LobbyTest, JoinRoom) {
    lobby->addPlayer(1, "Player1");
    lobby->addPlayer(2, "Player2");

    std::string roomId = lobby->createCustomRoom(1, "Test Room", 4, false);
    // Player 1 is already in the room (creator auto-joins)

    EXPECT_TRUE(lobby->joinRoom(2, roomId));

    auto room = roomManager->getRoom(roomId);
    EXPECT_EQ(room->getPlayerCount(), 2);
}

TEST_F(LobbyTest, JoinNonExistentRoom) {
    lobby->addPlayer(1, "Player1");
    EXPECT_FALSE(lobby->joinRoom(1, "non-existent"));
}

TEST_F(LobbyTest, RoomWorkflow) {
    lobby->addPlayer(1, "Player1");
    lobby->addPlayer(2, "Player2");
    lobby->addPlayer(3, "Player3");

    // Complete workflow: create -> join
    std::string roomId = lobby->createCustomRoom(1, "Workflow Room", 4, false);
    EXPECT_FALSE(roomId.empty());

    // Player 1 is already in the room (creator auto-joins), others join
    EXPECT_TRUE(lobby->joinRoom(2, roomId));
    EXPECT_TRUE(lobby->joinRoom(3, roomId));

    auto room = roomManager->getRoom(roomId);
    EXPECT_EQ(room->getPlayerCount(), 3);
}

TEST_F(LobbyTest, AddPlayer) {
    EXPECT_TRUE(lobby->addPlayer(1, "Alice"));
    EXPECT_TRUE(lobby->addPlayer(2, "Bob"));
    EXPECT_EQ(lobby->getPlayerCount(), 2);
}

TEST_F(LobbyTest, AddDuplicatePlayer) {
    EXPECT_TRUE(lobby->addPlayer(1, "Alice"));
    EXPECT_FALSE(lobby->addPlayer(1, "AliceDuplicate"));
    EXPECT_EQ(lobby->getPlayerCount(), 1);
}

TEST_F(LobbyTest, RemovePlayer) {
    lobby->addPlayer(1, "Alice");
    lobby->addPlayer(2, "Bob");

    EXPECT_TRUE(lobby->removePlayer(1));
    EXPECT_EQ(lobby->getPlayerCount(), 1);
}

TEST_F(LobbyTest, RemoveNonExistentPlayer) {
    EXPECT_FALSE(lobby->removePlayer(999));
}

TEST_F(LobbyTest, GetPlayer) {
    lobby->addPlayer(1, "TestPlayer");

    auto player = lobby->getPlayer(1);
    EXPECT_NE(player, nullptr);
    EXPECT_EQ(player->playerId, 1);
    EXPECT_EQ(player->playerName, "TestPlayer");
}

TEST_F(LobbyTest, GetNonExistentPlayer) {
    auto player = lobby->getPlayer(999);
    EXPECT_EQ(player, nullptr);
}

TEST_F(LobbyTest, GetAllPlayers) {
    lobby->addPlayer(1, "Alice");
    lobby->addPlayer(2, "Bob");
    lobby->addPlayer(3, "Charlie");

    auto players = lobby->getAllPlayers();
    EXPECT_EQ(players.size(), 3);
}

TEST_F(LobbyTest, StartMatchmaking) {
    lobby->addPlayer(1, "Alice");
    EXPECT_TRUE(lobby->startMatchmaking(1));
}

TEST_F(LobbyTest, CancelMatchmaking) {
    lobby->addPlayer(1, "Alice");
    lobby->startMatchmaking(1);
    EXPECT_TRUE(lobby->cancelMatchmaking(1));
}
