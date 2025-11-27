/*
** EPITECH PROJECT, 2025
** Created by samuelBleau on 26/11/2025.
** File description:
** Replicator.cpp
*/

#include "Replicator.hpp"

Replicator::Replicator(EventBus& eventBus) : _eventBus(eventBus) {
}

Replicator::~Replicator() {
    disconnect();
}

bool Replicator::connect(const std::string& host, uint16_t port) {
    _serverHost = host;
    _serverPort = port;
    _connected = false;
    return false;
}

void Replicator::disconnect() {
    _connected = false;
}

bool Replicator::isConnected() const {
    return _connected;
}

void Replicator::update() {
}

void Replicator::sendPacket(NetworkMessageType type, const std::vector<uint8_t>& data) {
}

uint32_t Replicator::getLatency() const {
    return _latency;
}

uint32_t Replicator::getPacketLoss() const {
    return _packetLoss;
}

void Replicator::onInputEvent(const InputEvent& event) {
}

void Replicator::processIncomingPacket(const std::vector<uint8_t>& packet) {
}