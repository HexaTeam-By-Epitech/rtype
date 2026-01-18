/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** MapLoader.hpp - Utility for loading map configurations from JSON files
*/

#pragma once

#include <fstream>
#include <memory>
#include <nlohmann/json.hpp>
#include <optional>
#include <string>
#include "common/ECS/Components/MapData.hpp"

namespace map {

    /**
     * @class MapLoader
     * @brief Utility class for loading map configurations from JSON files.
     * 
     * Parses JSON map configuration files and creates MapData components.
     * 
     * JSON format example:
     * {
     *   "mapId": "level_1",
     *   "name": "Zone Alpha",
     *   "scrollSpeed": 50.0,
     *   "background": "backgrounds/space_1.png",
     *   "spawnScript": "maps/level_1_spawn.lua",
     *   "duration": 120.0,
     *   "nextMap": "level_2"
     * }
     */
    class MapLoader {
       public:
        /**
         * @brief Load a map from a JSON file.
         * @param filePath Path to the JSON file (relative or absolute)
         * @return Optional containing MapData if successful, nullopt otherwise
         */
        static std::optional<ecs::MapData> loadFromFile(const std::string &filePath);

        /**
         * @brief Load a map from a JSON string.
         * @param jsonString JSON configuration string
         * @return Optional containing MapData if successful, nullopt otherwise
         */
        static std::optional<ecs::MapData> loadFromString(const std::string &jsonString);

        /**
         * @brief Parse JSON object into MapData.
         * @param jsonObj nlohmann::json object
         * @return Optional containing MapData if successful, nullopt otherwise
         */
        static std::optional<ecs::MapData> parseJson(const nlohmann::json &jsonObj);
    };

}  // namespace map
