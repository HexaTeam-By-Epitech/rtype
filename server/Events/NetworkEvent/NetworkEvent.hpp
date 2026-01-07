/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** NetworkEvent.hpp
*/

#pragma once

#include <string>
#include "server/Events/IEvent.hpp"

namespace server {

    /**
     * @class NetworkEvent
     * @brief Base class for all network-related events
     *
     * Represents events related to client-server communication.
     */
    class NetworkEvent : public IEvent {
       public:
        enum class Type { UNKNOWN, CLIENT_CONNECTED, CLIENT_DISCONNECTED, MESSAGE_RECEIVED, MESSAGE_SENT };

        explicit NetworkEvent(Type type, int clientId = -1);
        virtual ~NetworkEvent() override = default;

        /**
         * @brief Get the type of the network event
         */
        Type getType() const;

        /**
         * @brief Get the ID of the client related to this event
         */
        int getClientId() const;

       private:
        Type _type{Type::UNKNOWN};
        int _clientId{-1};
    };

}  // namespace server
