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
        // Get message type
        auto messageType = NetworkMessages::getMessageType(event.packet->getData());

        switch (messageType) {
            case NetworkMessages::MessageType::CONNECT_REQUEST: {
                // Parse connect request
                std::string playerName = NetworkMessages::parseConnectRequest(event.packet->getData());
                std::cout << "[Server] Player connected: " << playerName << std::endl;

                // Send response
                std::string responseMsg = "Hello " + playerName + ", welcome to R-Type!";
                auto responseData = NetworkMessages::createConnectResponse(responseMsg);
                auto packet = createPacket(responseData, static_cast<uint32_t>(PacketFlag::RELIABLE));
                event.peer->send(std::move(packet), 0);

                std::cout << "[Server] Sent welcome message to " << playerName << std::endl;
                break;
            }

            default:
                std::cout << "[Server] Received unknown message type" << std::endl;
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

    // Game loop (60 FPS)
    while (_running && _networkManager->isRunning()) {
        // Process network messages from network thread
        _networkManager->processMessages();

        // TODO: Update ECS systems
        // TODO: Broadcast world state to clients

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