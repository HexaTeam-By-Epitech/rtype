/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** IMatchmakingService.hpp
*/

#pragma once

#include <vector>

namespace server {

/**
 * @class IMatchmakingService
 * @brief Interface for a matchmaking system
 */
class IMatchmakingService {
public:
    virtual ~IMatchmakingService() = default;

    virtual void addPlayer(int playerId) = 0;
    virtual void removePlayer(int playerId) = 0;
    virtual void startMatch() = 0;
};

} // namespace server

