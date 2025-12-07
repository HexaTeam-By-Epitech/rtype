/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** SerializerFactory.cpp
*/

#include "SerializerFactory.hpp"
#include "capnp/CapnpSerializer.hpp"
#include "GameCodec.hpp"
#include "capnp/CapnpGameCodec.hpp"

std::unique_ptr<ISerializer> createDefaultSerializer() {
    return std::make_unique<CapnpSerializer>();
}

std::unique_ptr<IGameCodec> createDefaultGameCodec() {
    return std::make_unique<CapnpGameCodec>();
}