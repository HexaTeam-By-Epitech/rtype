/*
** EPITECH PROJECT, 2025
** Created by hugo on 06/12/2025
** File description:
** MatchmakingService.hpp
*/

#pragma once

#include "IMatchmakingService.hpp"

namespace server {

/**
 * @class MatchmakingService
 * @brief Concrete implementation of IMatchmakingService
 */
class MatchmakingService : public IMatchmakingService {
public:
    MatchmakingService() = default;
    ~MatchmakingService() override = default;

    void addPlayer(int playerId) override;
    void removePlayer(int playerId) override;
    void startMatch() override;

private:
    std::vector<int> _waitingPlayers;
};

} // namespace server

