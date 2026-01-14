/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** KickCommand.hpp
*/

#pragma once

#include <string>
#include <vector>
#include "ICommand.hpp"

namespace server {

    /**
     * @class KickCommand
     * @brief Kicks a player from the current room
     * 
     * Usage: /kick <player_id>
     * Only the room host can kick players.
     * Spectators cannot be kicked.
     * Works both in waiting room and during game.
     */
    class KickCommand : public ICommand {
       public:
        KickCommand() = default;
        ~KickCommand() override = default;

        std::string execute(const std::vector<std::string> &args, const CommandContext &context) override;
        std::string getName() const override { return "kick"; }
        std::string getDescription() const override { return "Kick a player from the room"; }
        std::string getUsage() const override { return "/kick <player_id>"; }
    };

}  // namespace server
