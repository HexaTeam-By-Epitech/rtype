/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** UIEvent.hpp - Events triggered by UI interactions
*/

#pragma once

#include <string>
#include <vector>
#include "Events/IEvent.hpp"

// Room data structure for events
struct RoomData {
    std::string roomId;
    std::string roomName;
    uint32_t playerCount;
    uint32_t maxPlayers;
    bool isPrivate;
    uint8_t state;
};

enum class UIEventType {
    JOIN_GAME,
    START_GAME,
    QUIT_GAME,
    SERVER_CONNECT,
    CONNECTION_FAILED,
    CONNECTION_SUCCESS,
    CREATE_ROOM,
    REQUEST_ROOM_LIST,
    ROOM_LIST_RECEIVED,
    ROOM_JOINED,
    LEAVE_ROOM,
    PLAYER_JOINED_ROOM,
    START_GAME_REQUEST,
    GAME_STARTED,
    REGISTER_ACCOUNT,  // data format: "username:password"
    REGISTER_SUCCESS,  // data format: success message
    REGISTER_FAILED,   // data format: error message
    LOGIN_ACCOUNT,     // data format: "username:password"
    LOGIN_FAILED,      // data format: error message
    AUTH_SUCCESS       // data format: authenticated username (e.g., "lucas" or "guest_a3f2")
};

class UIEvent : public IEvent {
   public:
    explicit UIEvent(UIEventType type, const std::string &data = "") : _type(type), _data(data) {}

    [[nodiscard]] UIEventType getType() const { return _type; }
    [[nodiscard]] const std::string &getData() const { return _data; }

   private:
    UIEventType _type;
    std::string _data;
};
