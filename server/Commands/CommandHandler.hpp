/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** CommandHandler.hpp
*/

#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>
#include "CommandContext.hpp"
#include "ICommand.hpp"

namespace server {

    /**
     * @class CommandHandler
     * @brief Central command dispatcher
     * 
     * Manages command registration, parsing and execution.
     * Follows the Command pattern with dependency injection.
     * 
     * Usage:
     *   CommandHandler handler;
     *   handler.registerCommand(std::make_shared<HelpCommand>());
     *   handler.registerCommand(std::make_shared<KickCommand>());
     *   
     *   std::string result = handler.handleCommand("/kick player1", context);
     */
    class CommandHandler {
       public:
        CommandHandler();
        ~CommandHandler() = default;

        /**
         * @brief Register a command
         * @param command Command instance to register
         */
        void registerCommand(std::shared_ptr<ICommand> command);

        /**
         * @brief Handle a command message
         * @param message Full message (including /)
         * @param context Execution context
         * @return Response message (empty if command not found)
         */
        std::string handleCommand(const std::string &message, const CommandContext &context);

        /**
         * @brief Check if a message is a command
         * @param message Message to check
         * @return true if message starts with /
         */
        static bool isCommand(const std::string &message);

        /**
         * @brief Get all registered commands
         * @return Vector of all commands
         */
        const std::vector<std::shared_ptr<ICommand>> &getCommands() const { return _commandList; }

       private:
        /**
         * @brief Parse command message into name and arguments
         * @param message Full command message
         * @return Pair of (command name, arguments)
         */
        std::pair<std::string, std::vector<std::string>> parseCommand(const std::string &message);

        std::unordered_map<std::string, std::shared_ptr<ICommand>> _commands;
        std::vector<std::shared_ptr<ICommand>> _commandList;  // For help command
    };

}  // namespace server
