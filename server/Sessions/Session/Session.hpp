/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** Session.hpp
*/

#pragma once

#include "ISession.hpp"

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
         * @brief Set session active state
         * @param active true to activate, false to deactivate
         */
        void setActive(bool active);

       private:
        std::string _id;
        uint32_t _playerId;
        bool _active{false};
    };

}  // namespace server
