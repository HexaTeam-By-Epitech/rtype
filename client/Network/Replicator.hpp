/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Replicator.hpp
*/

#ifndef REPLICATOR_HPP
#define REPLICATOR_HPP

#include <string>
#include <cstdint>
#include <functional>
#include <vector>
#include "../Core/EventBus/EventBus.hpp"
#include "../Events/NetworkEvent/NetworkEvent.hpp"
#include "../Events/InputEvent/InputEvent.hpp"

class Replicator {
public:

    explicit Replicator(EventBus& eventBus);
    ~Replicator();

    bool connect(const std::string& host, uint16_t port);
    void disconnect();
    bool isConnected() const;
    void update();
    void sendPacket(NetworkMessageType type, const std::vector<uint8_t>& data);
    uint32_t getLatency() const;
    uint32_t getPacketLoss() const;

private:
    void onInputEvent(const InputEvent& event);
    void processIncomingPacket(const std::vector<uint8_t>& packet);

    EventBus& _eventBus;
    bool _connected = false;
    std::string _serverHost;
    uint16_t _serverPort = 0;

    uint32_t _latency = 0;
    uint32_t _packetLoss = 0;
    
    // TODO: Ajouter socket UDP/TCP
};

#endif