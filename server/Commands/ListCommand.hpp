/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** ListCommand.hpp
*/

#pragma once

#include <string>
#include <vector>
#include "ICommand.hpp"

namespace server {

    /**
     * @class ListCommand
     * @brief Lists all players and spectators in the current room
     * 
     * Usage: /list
     * Displays:
     * - All active players in the room
     * - All spectators in the room
     * - Host indicator
     */
    class ListCommand : public ICommand {
       public:
        ListCommand() = default;
        ~ListCommand() override = default;

        std::string execute(const std::vector<std::string> &args, const CommandContext &context) override;
        std::string getName() const override { return "list"; }
        std::string getDescription() const override { return "List all players and spectators in the room"; }
        std::string getUsage() const override { return "/list"; }
    };

}  // namespace server
