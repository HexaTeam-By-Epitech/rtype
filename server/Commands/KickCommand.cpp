/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** KickCommand.cpp
*/

#include "server/Commands/KickCommand.hpp"
#include "common/Logger/Logger.hpp"
#include "server/Rooms/Room.hpp"
#include "server/Server/Server.hpp"

namespace server {

    std::string KickCommand::execute(const std::vector<std::string> &args, const CommandContext &context) {
        // Check if player is host
        if (context.room->getHost() != context.playerId) {
            return "Error: Only the room host can kick players.";
        }

        // Check arguments
        if (args.empty()) {
            return "Error: Missing player id. Usage: /kick <player_id>";
        }

        // Get all players in the room
        std::vector<uint32_t> players = context.room->getPlayers();

        try {
            if (args[0].empty() || std::stoul(args[0]) == 0) {
                return "Error: Invalid player id.";
            }
        } catch (std::invalid_argument &) {
            return "Error: Invalid player id.";
        }

        // Get target id from args[0] and cast it to uint32_t
        uint32_t targetId = std::stoul(args[0]);

        // Find target player by id
        uint32_t targetPlayerId = 0;
        for (uint32_t playerId : players) {
            if (playerId == targetId) {
                targetPlayerId = playerId;
                break;
            }
        }

        if (targetPlayerId == 0) {
            return "Error: Player not found in this room.";
        }

        // Check if trying to kick self
        if (targetPlayerId == context.playerId) {
            return "Error: You cannot kick yourself. Use /leave to exit the room.";
        }

        // Check if target is a spectator (cannot kick spectators)
        if (context.room->hasSpectator(targetPlayerId)) {
            return "Error: Cannot kick spectators.";
        }

        // Kick the player
        LOG_INFO("Player ", context.playerName, " (", context.playerId, ") kicked ", targetId, " (",
                 targetPlayerId, ") from room ", context.room->getId());

        // Use server's kickPlayer method which handles all cleanup and notifications
        bool success = context.server->kickPlayer(targetPlayerId);

        if (!success) {
            return "Error: Failed to kick player from the room.";
        }

        return "Player has been kicked from the room.";
    }

}  // namespace server
