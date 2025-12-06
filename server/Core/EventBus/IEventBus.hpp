/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IEventBus.hpp
*/

#pragma once

namespace Server {

/**
 * @class IEventBus
 * @brief Interface minimale pour EventBus
 * 
 * Se limite à la méthode clear() qui n'est pas template.
 */
class IEventBus {
public:
    virtual ~IEventBus() = default;

    /**
     * @brief Clear all subscriptions
     */
    virtual void clear() = 0;
};

} // namespace Server

