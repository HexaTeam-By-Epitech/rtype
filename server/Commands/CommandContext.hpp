/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** CommandContext.hpp
*/

#pragma once

#include <cstdint>
#include <memory>
#include <string>

class Server;  // Forward declaration in global namespace

namespace server {
    class Room;

    /**
     * @struct CommandContext
     * @brief Context information for command execution
     * 
     * Contains all necessary information for a command to execute properly:
     * - Player information (ID, name)
     * - Room information
     * - Server reference for operations
     */
    struct CommandContext {
        uint32_t playerId;           ///< ID of the player executing the command
        std::string playerName;      ///< Name of the player executing the command
        std::shared_ptr<Room> room;  ///< Room where the command is executed
        ::Server *server;            ///< Server instance for operations (global namespace)

        CommandContext(uint32_t pId, const std::string &pName, std::shared_ptr<Room> r, ::Server *srv)
            : playerId(pId), playerName(pName), room(r), server(srv) {}
    };

}  // namespace server
