/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** MapLoader.cpp - Utility for loading map configurations from JSON files
*/

#include "MapLoader.hpp"
#include "common/Logger/Logger.hpp"

namespace map {

    std::optional<ecs::MapData> MapLoader::loadFromFile(const std::string &filePath) {
        try {
            std::ifstream file(filePath);
            if (!file.is_open()) {
                LOG_ERROR("Failed to open map file: ", filePath);
                return std::nullopt;
            }

            nlohmann::json jsonObj;
            file >> jsonObj;
            file.close();

            LOG_INFO("Loaded map configuration from: ", filePath);
            return parseJson(jsonObj);

        } catch (const nlohmann::json::exception &e) {
            LOG_ERROR("JSON parsing error in ", filePath, ": ", e.what());
            return std::nullopt;
        } catch (const std::exception &e) {
            LOG_ERROR("Error loading map from ", filePath, ": ", e.what());
            return std::nullopt;
        }
    }

    std::optional<ecs::MapData> MapLoader::loadFromString(const std::string &jsonString) {
        try {
            nlohmann::json jsonObj = nlohmann::json::parse(jsonString);
            return parseJson(jsonObj);

        } catch (const nlohmann::json::exception &e) {
            LOG_ERROR("JSON parsing error: ", e.what());
            return std::nullopt;
        } catch (const std::exception &e) {
            LOG_ERROR("Error loading map from string: ", e.what());
            return std::nullopt;
        }
    }

    std::optional<ecs::MapData> MapLoader::parseJson(const nlohmann::json &jsonObj) {
        try {
            // Required fields
            if (!jsonObj.contains("mapId")) {
                LOG_ERROR("Map JSON missing required field: mapId");
                return std::nullopt;
            }

            std::string mapId = jsonObj["mapId"].get<std::string>();

            // Optional fields with defaults
            std::string name = jsonObj.value("name", mapId);
            float scrollSpeed = jsonObj.value("scrollSpeed", 0.0f);
            std::string background = jsonObj.value("background", "");
            std::string spawnScript = jsonObj.value("spawnScript", "");
            float duration = jsonObj.value("duration", 0.0f);
            std::string nextMap = jsonObj.value("nextMap", "");

            // Create and return MapData
            ecs::MapData mapData(mapId, name, scrollSpeed, background, spawnScript, duration, nextMap);

            LOG_INFO("✓ Parsed map: '", name, "' (", mapId, ")");
            LOG_DEBUG("  - Scroll speed: ", scrollSpeed, " px/s");
            LOG_DEBUG("  - Duration: ", duration > 0.0f ? std::to_string(duration) + "s" : "infinite");
            if (!nextMap.empty()) {
                LOG_DEBUG("  - Next map: ", nextMap);
            }

            return mapData;

        } catch (const nlohmann::json::exception &e) {
            LOG_ERROR("Error parsing map JSON: ", e.what());
            return std::nullopt;
        } catch (const std::exception &e) {
            LOG_ERROR("Unexpected error parsing map: ", e.what());
            return std::nullopt;
        }
    }

}  // namespace map
