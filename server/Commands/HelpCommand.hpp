/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** HelpCommand.hpp
*/

#pragma once

#include <memory>
#include <string>
#include <vector>
#include "ICommand.hpp"

namespace server {

    /**
     * @class HelpCommand
     * @brief Displays available commands to the player
     * 
     * Usage: /help
     * Lists all registered commands with their descriptions.
     */
    class HelpCommand : public ICommand {
       public:
        HelpCommand(const std::vector<std::shared_ptr<ICommand>> &commands) : _commands(commands) {}
        ~HelpCommand() override = default;

        std::string execute(const std::vector<std::string> &args, const CommandContext &context) override;
        std::string getName() const override { return "help"; }
        std::string getDescription() const override { return "Display available commands"; }
        std::string getUsage() const override { return "/help"; }

       private:
        const std::vector<std::shared_ptr<ICommand>> &_commands;
    };

}  // namespace server
