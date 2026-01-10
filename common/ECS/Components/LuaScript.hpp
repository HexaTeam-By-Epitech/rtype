/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** LuaScript
*/

#pragma once

#include <string>
#include "IComponent.hpp"

namespace ecs {
    /**
     * @class LuaScript
     * @brief Component that holds the path to a Lua script for entity behavior.
     * 
     * This component allows entities to be associated with Lua scripts,
     * enabling dynamic behavior defined in external script files.
     */
    class LuaScript : public IComponent {
       public:
        /**
         * @brief Default constructor.
         * Initializes with an empty script path.
         */
        LuaScript() : _scriptPath("") {}

        /**
         * @brief Constructor with script path.
         * @param scriptPath Path to the Lua script file.
         */
        explicit LuaScript(const std::string &scriptPath) : _scriptPath(scriptPath) {}
        ~LuaScript() override = default;

        /**
         * @brief Get the path to the Lua script.
         * @return const std::string& The script file path.
         */
        const std::string &getScriptPath() const { return _scriptPath; }

        /**
         * @brief Set the path to the Lua script.
         * @param scriptPath New script file path.
         */
        void setScriptPath(const std::string &scriptPath) { _scriptPath = scriptPath; }

        /**
         * @brief Get the unique type identifier for this component.
         * @return ComponentType The unique ID of this component type.
         */
        ComponentType getType() const override { return getComponentType<LuaScript>(); }

       private:
        std::string _scriptPath; /**< Path to the Lua script file */
    };
}  // namespace ecs
