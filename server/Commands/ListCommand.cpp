/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** ListCommand.cpp
*/

#include "server/Commands/ListCommand.hpp"
#include <sstream>
#include "server/Rooms/Room.hpp"
#include "server/Server/Server.hpp"

namespace server {

    std::string ListCommand::execute(const std::vector<std::string> &args, const CommandContext &context) {
        (void)args;

        std::ostringstream oss;
        uint32_t hostId = context.room->getHost();

        // List players
        std::vector<uint32_t> players = context.room->getPlayers();
        oss << "=== Room " << context.room->getId() << " ===";
        oss << "\nPlayers (" << players.size() << "/" << context.room->getMaxPlayers() << "): ";

        if (players.empty()) {
            oss << "(none)";
        } else {
            for (uint32_t playerId : players) {

                const LobbyPlayer *player = context.server->getLobby()->getPlayer(playerId);
                std::string playerName = player ? player->playerName : ("Player" + std::to_string(playerId));

                oss << "\n - " << playerName << " (id: " << playerId << ")";
                if (playerId == hostId) {
                    oss << "*";  // * indicates host
                }
            }
        }

        // List spectators
        std::vector<uint32_t> spectators = context.room->getSpectators();
        oss << "\nSpectators (" << spectators.size() << "): ";

        if (spectators.empty()) {
            oss << "(none)";
        } else {
            bool first = true;
            for (uint32_t spectatorId : spectators) {
                if (!first)
                    oss << ", ";
                first = false;

                const server::LobbyPlayer *spectator = context.server->getLobby()->getPlayer(spectatorId);
                std::string spectatorName =
                    spectator ? spectator->playerName : ("Spectator" + std::to_string(spectatorId));

                oss << spectatorName;
            }
        }

        return oss.str();
    }

}  // namespace server
