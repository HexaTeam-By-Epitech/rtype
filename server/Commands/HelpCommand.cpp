/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** HelpCommand.cpp
*/

#include "HelpCommand.hpp"
#include <sstream>

namespace server {

    std::string HelpCommand::execute(const std::vector<std::string> &args, const CommandContext &context) {
        (void)args;
        (void)context;

        std::ostringstream oss;
        oss << "=== Available Commands ===";

        for (const auto &cmd : _commands) {
            oss << "\n" << cmd->getUsage() << " — " << cmd->getDescription();
        }

        return oss.str();
    }

}  // namespace server
