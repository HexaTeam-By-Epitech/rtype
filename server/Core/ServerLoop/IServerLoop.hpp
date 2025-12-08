/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IServerLoop.hpp
*/

#pragma once

#include <memory>

namespace server {

    /**
     * @interface IServerLoop
     * @brief Interface for the main server loop
     *
     * Provides methods to start, stop, and check the running status of the server loop.
     */
    class IServerLoop {
       public:
        virtual ~IServerLoop() = default;

        /**
         * @brief Start the server loop
         */
        virtual void start() = 0;

        /**
         * @brief Stop the server loop
         */
        virtual void stop() = 0;

        /**
         * @brief Check if the server loop is running
         * @return true if running, false otherwise
         */
        virtual bool isRunning() const = 0;
    };

}  // namespace server
