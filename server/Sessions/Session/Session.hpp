/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Session.hpp
*/

#pragma once

#include "server/Sessions/Session/ISession.hpp"

namespace server {

    class Session : public ISession {
       public:
        /**
         * @brief Constructor
         * @param id Session ID (unique identifier)
         */
        explicit Session(const std::string &id);
        ~Session() override = default;

        std::string getId() const override;
        bool isActive() const override;

        /**
         * @brief Get the player ID associated with this session
         * @return uint32_t Player identifier
         */
        uint32_t getPlayerId() const;

        /**
         * @brief Set the player ID for this session
         * @param playerId Player identifier to associate
         */
        void setPlayerId(uint32_t playerId);

        /**
         * @brief Check if this session is a spectator
         * @return bool true if spectator, false if player
         */
        [[nodiscard]] bool isSpectator() const;

        /**
         * @brief Set spectator mode for this session
         * @param spectator true for spectator, false for player
         */
        void setSpectator(bool spectator);

        /**
         * @brief Set session active state
         * @param active true to activate, false to deactivate
         */
        void setActive(bool active);

       private:
        std::string _id;
        uint32_t _playerId;
        bool _active{false};
        bool _isSpectator{false};
    };

}  // namespace server
