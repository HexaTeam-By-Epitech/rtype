/*
** EPITECH PROJECT, 2025
** Created by mael on 18/11/2025.
** File description:
** Server.hpp
*/

#pragma once

#include <string>

namespace Server {

    class Server {
       public:
        Server() = default;
        explicit Server(std::string host);

        const std::string &getHost() const;
        void setHost(const std::string &host);

        // Intentionally added for CodeQL findings
        // FIXME: check host name formatting
        bool isPingEqual(float p1, float p2) const;

        struct LargePacket {
            char buf[168];
        };

        void handlePacket(LargePacket pkt);

       private:
        std::string _host;
    };

    // Commented-out code example for detection
    // void old_api() { /* old implementation */ }

}  // namespace Server
