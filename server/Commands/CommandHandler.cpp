/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** CommandHandler.cpp
*/

#include "CommandHandler.hpp"
#include <sstream>
#include "CommandContext.hpp"

#include <memory>
#include "../../common/Logger/Logger.hpp"
#include "HelpCommand.hpp"
#include "ICommand.hpp"
#include "KickCommand.hpp"
#include "ListCommand.hpp"

namespace server {

    CommandHandler::CommandHandler() {
        // Register built-in commands
        registerCommand(std::make_shared<KickCommand>());
        registerCommand(std::make_shared<ListCommand>());

        // HelpCommand needs access to all commands, so we register it after
        registerCommand(std::make_shared<HelpCommand>(_commandList));
    }

    void CommandHandler::registerCommand(std::shared_ptr<ICommand> command) {
        std::string name = command->getName();
        _commands[name] = command;
        _commandList.push_back(command);
        LOG_DEBUG("Registered command: /", name);
    }

    bool CommandHandler::isCommand(const std::string &message) {
        return !message.empty() && message[0] == '/';
    }

    std::pair<std::string, std::vector<std::string>> CommandHandler::parseCommand(
        const std::string &message) {
        std::istringstream iss(message.substr(1));  // Skip the '/'
        std::string commandName;
        std::vector<std::string> args;

        iss >> commandName;  // First word is command name

        // Rest are arguments
        std::string arg;
        while (iss >> arg) {
            args.push_back(arg);
        }

        return {commandName, args};
    }

    std::string CommandHandler::handleCommand(const std::string &message, const CommandContext &context) {
        if (!isCommand(message)) {
            return "";
        }

        auto [commandName, args] = parseCommand(message);

        // Find command
        auto it = _commands.find(commandName);
        if (it == _commands.end()) {
            return "Unknown command: /" + commandName + ". Type /help for available commands.";
        }

        // Execute command
        try {
            std::string result = it->second->execute(args, context);
            LOG_DEBUG("Command /", commandName, " executed by player ", context.playerId);
            return result;
        } catch (const std::exception &e) {
            LOG_ERROR("Command execution failed: ", e.what());
            return "Command execution failed: " + std::string(e.what());
        }
    }

}  // namespace server
