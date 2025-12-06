/*
** EPITECH PROJECT, 2025
** Created by mael on 02/12/2025.
** File description:
** SerializerFactory.hpp
*/

#pragma once

#include "ISerializer.hpp"
#include <memory>

/**
 * @brief Create the default serializer used by the engine.
 *
 * This function is the main entry point for client/server code. It returns a
 * concrete implementation of ISerializer (currently Cap'n Proto) while
 * keeping the caller fully agnostic of the backend.
 *
 * @return Unique pointer to an ISerializer instance ready to use.
 */
std::unique_ptr<ISerializer> createDefaultSerializer();