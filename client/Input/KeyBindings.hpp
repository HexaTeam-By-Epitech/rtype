/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** KeyBindings - Centralized key binding management
*/

#pragma once

#include <raylib.h>
#include <functional>
#include <string>
#include <unordered_map>

namespace Input {

    /**
     * @brief Enumeration of all bindable game actions
     */
    enum class GameAction {
        // Movement
        MOVE_UP,
        MOVE_DOWN,
        MOVE_LEFT,
        MOVE_RIGHT,

        // Combat
        SHOOT,

        // UI/System
        PAUSE_MENU,
        CHAT_OPEN,

        // Menu Navigation
        MENU_NEXT,
        MENU_PREVIOUS,
        MENU_CONFIRM,
        MENU_BACK,

        COUNT  // Keep last for iteration
    };

    /**
     * @brief Centralized key binding manager (Singleton)
     *
     * Manages all game key bindings with support for:
     * - Multiple keys per action (primary/secondary)
     * - Runtime remapping
     * - Persistence (save/load)
     * - Human-readable key names
     */
    class KeyBindings {
       public:
        /**
         * @brief Get the singleton instance
         * @return Reference to the KeyBindings instance
         */
        static KeyBindings &getInstance();

        // Delete copy/move constructors
        KeyBindings(const KeyBindings &) = delete;
        KeyBindings &operator=(const KeyBindings &) = delete;
        KeyBindings(KeyBindings &&) = delete;
        KeyBindings &operator=(KeyBindings &&) = delete;

        /**
         * @brief Reset all bindings to defaults
         */
        void ResetToDefaults();

        /**
         * @brief Get the primary key for an action
         * @param action The game action
         * @return The key code (raylib KEY_* constant)
         */
        [[nodiscard]] int GetPrimaryKey(GameAction action) const;

        /**
         * @brief Get the secondary key for an action
         * @param action The game action
         * @return The key code, or KEY_NULL if not set
         */
        [[nodiscard]] int GetSecondaryKey(GameAction action) const;

        /**
         * @brief Set the primary key for an action
         * @param action The game action
         * @param key The key code to bind
         */
        void SetPrimaryKey(GameAction action, int key);

        /**
         * @brief Set the secondary key for an action
         * @param action The game action
         * @param key The key code to bind
         */
        void SetSecondaryKey(GameAction action, int key);

        /**
         * @brief Clear the secondary key for an action
         * @param action The game action
         */
        void ClearSecondaryKey(GameAction action);

        /**
         * @brief Check if a key is bound to an action (primary or secondary)
         * @param action The game action to check
         * @param key The key code to check
         * @return true if the key is bound to this action
         */
        [[nodiscard]] bool IsKeyBoundToAction(GameAction action, int key) const;

        /**
         * @brief Get human-readable name of a key
         * @param key The key code
         * @return String representation of the key
         */
        [[nodiscard]] static std::string GetKeyName(int key);

        /**
         * @brief Get human-readable name of an action
         * @param action The game action
         * @return String representation of the action
         */
        [[nodiscard]] static std::string GetActionName(GameAction action);

        /**
         * @brief Set callback for when bindings change
         * @param callback Function to call when any binding changes
         */
        void SetOnBindingsChanged(std::function<void()> callback);

        /**
         * @brief Save bindings to a file
         * @param filepath Path to save file
         * @return true if successful
         */
        bool SaveToFile(const std::string &filepath) const;

        /**
         * @brief Load bindings from a file
         * @param filepath Path to load file
         * @return true if successful
         */
        bool LoadFromFile(const std::string &filepath);

       private:
        KeyBindings();
        ~KeyBindings() = default;

        struct KeyBinding {
            int primary{KEY_NULL};
            int secondary{KEY_NULL};
        };

        std::unordered_map<GameAction, KeyBinding> _bindings;
        std::function<void()> _onBindingsChanged;

        void NotifyBindingsChanged();
    };

}  // namespace Input
