/*
** EPITECH PROJECT, 2025
** Created on 09/12/2025.
** File description:
** Messages.hpp
*/

#pragma once

// Connection messages (handshake, ping/pong)
#include "Connection/Connection.hpp"

// Shared types
#include "Shared/SharedTypes.hpp"

// Client to Server messages
#include "C2S/C2S.hpp"

// Server to Client messages
#include "S2C/S2C.hpp"

/**
 * @namespace RType::Messages
 * @brief All game messages for R-Type network protocol
 * 
 * Architecture:
 * - RType::Messages::Connection - Connection protocol (handshake, ping/pong)
 * - RType::Messages::Shared - Common types (Vec2, Action, EntityType, etc.)
 * - RType::Messages::C2S - Client-to-Server messages
 * - RType::Messages::S2C - Server-to-Client messages
 * 
 * Each message class provides:
 * - serialize() - Convert to byte vector for network transmission
 * - deserialize() - Parse byte vector back to C++ object
 * 
 * Usage example:
 * 
 *   #include "Messages/Messages.hpp"
 *   using namespace RType::Messages;
 * 
 *   // Create and send a message
 *   C2S::PlayerInput input(123, {Shared::Action::MoveUp, Shared::Action::Shoot});
 *   auto payload = input.serialize();
 *   
 *   // Wrap in network protocol
 *   auto packet = NetworkMessages::createMessage(
 *       NetworkMessages::MessageType::C2S_PLAYER_INPUT,
 *       payload
 *   );
 *   
 *   // Send packet over network...
 * 
 *   // Receive and deserialize
 *   auto type = NetworkMessages::getMessageType(receivedPacket);
 *   auto payload = NetworkMessages::getPayload(receivedPacket);
 *   auto input = C2S::PlayerInput::deserialize(payload);
 */
