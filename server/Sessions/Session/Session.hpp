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
        Session() = default;
        ~Session() override = default;

        std::string getId() const override;
        bool isActive() const override;

       private:
        std::string _id;
        bool _active{false};
    };

}  // namespace server
