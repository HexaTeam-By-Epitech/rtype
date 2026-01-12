/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** UIEvent.hpp - Events triggered by UI interactions
*/

#pragma once

#include <string>
#include "Events/IEvent.hpp"

enum class UIEventType { JOIN_GAME, START_GAME, QUIT_GAME };

class UIEvent : public IEvent {
   public:
    explicit UIEvent(UIEventType type, const std::string &data = "") : _type(type), _data(data) {}

    [[nodiscard]] UIEventType getType() const { return _type; }
    [[nodiscard]] const std::string &getData() const { return _data; }

   private:
    UIEventType _type;
    std::string _data;
};
