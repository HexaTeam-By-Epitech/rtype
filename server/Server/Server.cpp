/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** Server.cpp
*/

#include "Server.hpp"
#include <chrono>
#include <iostream>
#include <thread>
#include "Capnp/Messages/Messages.hpp"  // Cap'n Proto DTOs
#include "Capnp/NetworkMessages.hpp"
#include "NetworkFactory.hpp"

Server::Server(uint16_t port, size_t maxClients) : _port(port), _maxClients(maxClients) {}

Server::~Server() {
    std::cout << "[Server] Shutting down..." << std::endl;
    stop();
    deinitializeNetworking();
}

bool Server::initialize() {
    if (_initialized) {
        return true;
    }

    std::cout << "[Server] Initializing R-Type server..." << std::endl;

    // Initialize networking
    if (!initializeNetworking()) {
        std::cerr << "[Server] Failed to initialize networking" << std::endl;
        return false;
    }

    // Create network manager
    _networkManager = std::make_unique<ServerNetworkManager>(_port, _maxClients);
    std::cout << "[Server] ✓ Network manager created" << std::endl;

    // Set packet handler
    _networkManager->setPacketHandler([this](HostNetworkEvent &event) { this->handlePacket(event); });
    std::cout << "[Server] ✓ Packet handler set" << std::endl;

    // Start network thread
    if (!_networkManager->start()) {
        std::cerr << "[Server] Failed to start network manager" << std::endl;
        return false;
    }
    std::cout << "[Server] ✓ Network thread started" << std::endl;

    _initialized = true;
    std::cout << "[Server] Initialization complete!" << std::endl;

    return true;
}

void Server::handlePacket(HostNetworkEvent &event) {
    if (!event.packet || !event.peer) {
        return;
    }

    try {
        using namespace RType::Messages;

        // Get message type
        auto messageType = NetworkMessages::getMessageType(event.packet->getData());

        switch (messageType) {
            case NetworkMessages::MessageType::HANDSHAKE_REQUEST: {
                // Parse connect request
                std::string playerName = NetworkMessages::parseConnectRequest(event.packet->getData());
                std::cout << "[Server] Player '" << playerName << "' requesting to join..." << std::endl;

                // Assign unique entity ID to new player
                static std::atomic<uint32_t> nextPlayerId{1000};  // Start player IDs at 1000
                uint32_t newPlayerId = nextPlayerId.fetch_add(1);

                std::cout << "[Server] Creating player entity (ID: " << newPlayerId << ")" << std::endl;

                // ============================================================
                // CREATE GAMESTART MESSAGE
                // ============================================================
                S2C::GameStart gameStart;
                gameStart.yourEntityId = newPlayerId;
                gameStart.initialState.serverTick = 0;

                // ============================================================
                // ADD NEW PLAYER TO INITIAL STATE
                // ============================================================
                S2C::EntityState newPlayerEntity;
                newPlayerEntity.entityId = newPlayerId;
                newPlayerEntity.type = Shared::EntityType::Player;
                newPlayerEntity.position = Shared::Vec2(50.0f, 300.0f);  // Spawn on left side
                newPlayerEntity.health = 100;
                gameStart.initialState.entities.push_back(newPlayerEntity);

                // TODO: Query ECS or use a set of active player IDs to populate initialState.entities with real players only.
                // The following placeholder logic is removed because it sends phantom players:
                // for (uint32_t id = 1000; id < newPlayerId; ++id) {
                //     S2C::EntityState otherPlayer;
                //     otherPlayer.entityId = id;
                //     otherPlayer.type = Shared::EntityType::Player;
                //     otherPlayer.position = Shared::Vec2(50.0F, 200.0F + 50.0F * (id - 1000));
                //     otherPlayer.health = 85;
                //     gameStart.initialState.entities.push_back(otherPlayer);
                // }

                // ============================================================
                // ENTITY ID RANGES (documented):
                //   Players: 1000-1999
                //   Enemies: 2000-2999
                //   Bullets: 3000-3999
                // ============================================================
                // ADD ENEMIES (example game state)
                // ============================================================
                S2C::EntityState enemy1;
                enemy1.entityId = 2001;
                enemy1.type = Shared::EntityType::EnemyType1;
                enemy1.position = Shared::Vec2(600.0f, 150.0f);
                enemy1.health = 50;
                gameStart.initialState.entities.push_back(enemy1);

                S2C::EntityState enemy2;
                enemy2.entityId = 2002;
                enemy2.type = Shared::EntityType::EnemyType1;
                enemy2.position = Shared::Vec2(700.0f, 400.0f);
                enemy2.health = 50;
                gameStart.initialState.entities.push_back(enemy2);

                // ============================================================
                // ADD BULLETS (example)
                // ============================================================
                S2C::EntityState bullet;
                bullet.entityId = 3001;
                bullet.type = Shared::EntityType::PlayerBullet;
                bullet.position = Shared::Vec2(250.0f, 300.0f);
                bullet.health = -1;  // No health: -1 is sentinel value for 'no health' in serialization
                gameStart.initialState.entities.push_back(bullet);

                // ============================================================
                // SERIALIZE AND SEND
                // ============================================================
                auto gameStartPayload = gameStart.serialize();
                auto gameStartPacket = NetworkMessages::createMessage(
                    NetworkMessages::MessageType::S2C_GAME_START, gameStartPayload);
                auto packet = createPacket(gameStartPacket, static_cast<uint32_t>(PacketFlag::RELIABLE));
                event.peer->send(std::move(packet), 0);

                std::cout << "[Server] ✓ Sent GameStart to '" << playerName << "'" << std::endl;
                std::cout << "[Server]   - Your entity ID: " << newPlayerId << std::endl;
                std::cout << "[Server]   - Total entities: " << gameStart.initialState.entities.size()
                          << std::endl;
                std::cout << "[Server]   - Players: " << (nextPlayerId - 1000) << std::endl;
                std::cout << "[Server]   - Enemies: 2" << std::endl;
                std::cout << "[Server]   - Bullets: 1" << std::endl;
                break;
            }

            case NetworkMessages::MessageType::C2S_PLAYER_INPUT: {
                // TODO: Handle player input
                auto payload = NetworkMessages::getPayload(event.packet->getData());
                auto input = C2S::PlayerInput::deserialize(payload);

                std::cout << "[Server] Received input from client (seq: " << input._sequenceId
                          << ", actions: " << input.actions.size() << ")" << std::endl;
                break;
            }

            default:
                std::cout << "[Server] Received unknown message type: " << static_cast<int>(messageType)
                          << std::endl;
                break;
        }

    } catch (const std::exception &e) {
        std::cerr << "[Server] Error handling packet: " << e.what() << std::endl;
    }
}

void Server::run() {
    if (!_initialized) {
        std::cerr << "[Server] Cannot run: not initialized" << std::endl;
        return;
    }

    std::cout << "[Server] ========================================" << std::endl;
    std::cout << "[Server] R-Type server running!" << std::endl;
    std::cout << "[Server] Port: " << _port << std::endl;
    std::cout << "[Server] Max clients: " << _maxClients << std::endl;
    std::cout << "[Server] Architecture:" << std::endl;
    std::cout << "[Server]   THREAD 1: Network (accepting connections)" << std::endl;
    std::cout << "[Server]   THREAD 2: Game loop (ECS at 60 FPS)" << std::endl;
    std::cout << "[Server] Press Ctrl+C to stop" << std::endl;
    std::cout << "[Server] ========================================" << std::endl;

    _running = true;
    uint32_t currentTick = 0;
    uint32_t lastBroadcastTick = 0;
    const uint32_t BROADCAST_INTERVAL = 3;  // Broadcast every 3 ticks (~50ms at 60 FPS)

    // Game loop (60 FPS)
    while (_running && _networkManager->isRunning()) {
        // Process network messages from network thread
        _networkManager->processMessages();

        // TODO: Update ECS systems (physics, AI, collisions, etc.)

        // ============================================================
        // BROADCAST GAME STATE TO ALL CLIENTS
        // ============================================================
        if (currentTick - lastBroadcastTick >= BROADCAST_INTERVAL) {
            using namespace RType::Messages;

            S2C::GameState state;
            state.serverTick = currentTick;

            // TODO: Query ECS for all entities
            // For now, add example entities (moving enemy)
            S2C::EntityState enemy;
            enemy.entityId = 2003;  // Use a different ID than GameStart demo enemies to avoid duplication
            enemy.type = Shared::EntityType::EnemyType1;
            // Simulate movement: enemy moves left
            float enemyX = 600.0f - (currentTick * 0.5f);
            if (enemyX < 0) {
                enemyX = 800.0f;  // Wrap around
            }
            enemy.position = Shared::Vec2(enemyX, 150.0f);
            enemy.health = 50;
            state.entities.push_back(enemy);

            // Serialize and broadcast
            auto payload = state.serialize();
            auto packet =
                NetworkMessages::createMessage(NetworkMessages::MessageType::S2C_GAME_STATE, payload);

            // TODO: Broadcast to all connected clients
            // _networkManager->broadcastToAll(packet);

            lastBroadcastTick = currentTick;
        }

        currentTick++;

        // Sleep to maintain ~60 FPS
        std::this_thread::sleep_for(std::chrono::milliseconds(16));
    }

    std::cout << "[Server] Game loop stopped." << std::endl;
}

void Server::stop() {
    std::cout << "[Server] Stop requested..." << std::endl;
    _running = false;

    if (_networkManager) {
        _networkManager->stop();
    }
}