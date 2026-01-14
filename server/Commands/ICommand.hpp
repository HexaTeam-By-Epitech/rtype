/*
** EPITECH PROJECT, 2025
** Created on 14/01/2026.
** File description:
** ICommand.hpp
*/

#pragma once

#include <string>
#include <vector>
#include "CommandContext.hpp"

namespace server {

    /**
     * @class ICommand
     * @brief Interface for all chat commands
     * 
     * Defines the contract for all command implementations.
     * Each command must be able to execute with given arguments and context.
     */
    class ICommand {
       public:
        virtual ~ICommand() = default;

        /**
         * @brief Execute the command
         * @param args Command arguments (without the command name)
         * @param context Execution context with player and room information
         * @return Response message to send to the player
         */
        virtual std::string execute(const std::vector<std::string> &args, const CommandContext &context) = 0;

        /**
         * @brief Get command name
         * @return The command name (without /)
         */
        virtual std::string getName() const = 0;

        /**
         * @brief Get command description
         * @return Description of what the command does
         */
        virtual std::string getDescription() const = 0;

        /**
         * @brief Get command usage
         * @return Usage example/syntax for the command
         */
        virtual std::string getUsage() const = 0;
    };

}  // namespace server
