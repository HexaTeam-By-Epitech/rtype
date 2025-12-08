/*
** EPITECH PROJECT, 2025
** Created by mael on 08/12/2025.
** File description:
** ServerNetworkManager.hpp
*/

#pragma once

#include <atomic>
#include <functional>
#include <memory>
#include <thread>
#include "Capnp/NetworkMessages.hpp"
#include "IHost.hpp"
#include "NetworkFactory.hpp"
#include "ThreadSafeQueue.hpp"

/**
 * @class ServerNetworkManager
 * @brief Manages network communication for the server with dedicated thread
 * 
 * Architecture:
 * ```
 * NETWORK THREAD                    GAME THREAD (ECS)
 * ┌──────────────────┐             ┌──────────────────┐
 * │ networkLoop()    │             │ processMessages  │
 * │   service()      │──[Queue]───>│   handlePacket   │
 * │   push event     │             │   update ECS     │
 * └──────────────────┘             └──────────────────┘
 * ```
 */
class ServerNetworkManager {
   public:
    /**
     * @brief Packet handler callback type
     * @param event Network event with packet data
     */
    using PacketHandler = std::function<void(HostNetworkEvent &)>;

    /**
     * @brief Constructor
     * @param port Port to listen on
     * @param maxClients Maximum number of clients
     */
    ServerNetworkManager(uint16_t port, size_t maxClients = 32);

    /**
     * @brief Destructor - stops network thread
     */
    ~ServerNetworkManager();

    /**
     * @brief Start the server and network thread
     * @return true if started successfully
     */
    bool start();

    /**
     * @brief Stop the server and network thread
     */
    void stop();

    /**
     * @brief Process incoming network events from the queue
     * 
     * Must be called from the game thread every frame.
     * Processes all available events and calls the registered handlers.
     */
    void processMessages();

    /**
     * @brief Set packet handler callback
     * @param handler Function to call when a packet is received
     */
    void setPacketHandler(PacketHandler handler) { _packetHandler = handler; }

    /**
     * @brief Check if server is running
     */
    bool isRunning() const { return _running; }

   private:
    /**
     * @brief Network thread main loop
     * 
     * Continuously polls for network events and pushes them to the queue.
     */
    void networkThreadLoop();

    uint16_t _port;
    size_t _maxClients;
    std::unique_ptr<IHost> _host;

    // Multi-threading components
    std::thread _networkThread;
    std::atomic<bool> _running{false};
    ThreadSafeQueue<HostNetworkEvent> _eventQueue;

    // Callback
    PacketHandler _packetHandler;
};