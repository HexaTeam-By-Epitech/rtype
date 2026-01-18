/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** KeyBindings - Centralized key binding management implementation
*/

#include "Input/KeyBindings.hpp"
#include <fstream>
#include <sstream>
#include "../common/Logger/Logger.hpp"

namespace Input {

    KeyBindings &KeyBindings::getInstance() {
        static KeyBindings instance;
        return instance;
    }

    KeyBindings::KeyBindings() {
        ResetToDefaults();
    }

    void KeyBindings::ResetToDefaults() {
        _bindings.clear();

        // Movement - WASD primary, ZQSD secondary (French layout support)
        _bindings[GameAction::MOVE_UP] = {KEY_W, KEY_Z};
        _bindings[GameAction::MOVE_DOWN] = {KEY_S, KEY_NULL};
        _bindings[GameAction::MOVE_LEFT] = {KEY_A, KEY_Q};
        _bindings[GameAction::MOVE_RIGHT] = {KEY_D, KEY_NULL};

        // Combat
        _bindings[GameAction::SHOOT] = {KEY_SPACE, KEY_NULL};

        // UI/System
        _bindings[GameAction::PAUSE_MENU] = {KEY_ESCAPE, KEY_NULL};
        _bindings[GameAction::CHAT_OPEN] = {KEY_T, KEY_ENTER};

        // Menu Navigation
        _bindings[GameAction::MENU_NEXT] = {KEY_TAB, KEY_DOWN};
        _bindings[GameAction::MENU_PREVIOUS] = {KEY_UP, KEY_NULL};  // Shift+Tab handled specially
        _bindings[GameAction::MENU_CONFIRM] = {KEY_ENTER, KEY_KP_ENTER};
        _bindings[GameAction::MENU_BACK] = {KEY_ESCAPE, KEY_BACKSPACE};

        NotifyBindingsChanged();
    }

    int KeyBindings::GetPrimaryKey(GameAction action) const {
        auto it = _bindings.find(action);
        if (it != _bindings.end()) {
            return it->second.primary;
        }
        return KEY_NULL;
    }

    int KeyBindings::GetSecondaryKey(GameAction action) const {
        auto it = _bindings.find(action);
        if (it != _bindings.end()) {
            return it->second.secondary;
        }
        return KEY_NULL;
    }

    void KeyBindings::SetPrimaryKey(GameAction action, int key) {
        _bindings[action].primary = key;
        NotifyBindingsChanged();
    }

    void KeyBindings::SetSecondaryKey(GameAction action, int key) {
        _bindings[action].secondary = key;
        NotifyBindingsChanged();
    }

    void KeyBindings::ClearSecondaryKey(GameAction action) {
        _bindings[action].secondary = KEY_NULL;
        NotifyBindingsChanged();
    }

    bool KeyBindings::IsKeyBoundToAction(GameAction action, int key) const {
        auto it = _bindings.find(action);
        if (it != _bindings.end()) {
            return it->second.primary == key || it->second.secondary == key;
        }
        return false;
    }

    std::string KeyBindings::GetKeyName(int key) {
        // Common keys
        static const std::unordered_map<int, std::string> keyNames = {
            {KEY_NULL, "None"},
            {KEY_SPACE, "Space"},
            {KEY_ESCAPE, "Escape"},
            {KEY_ENTER, "Enter"},
            {KEY_KP_ENTER, "Numpad Enter"},
            {KEY_TAB, "Tab"},
            {KEY_BACKSPACE, "Backspace"},
            {KEY_DELETE, "Delete"},
            {KEY_INSERT, "Insert"},
            {KEY_HOME, "Home"},
            {KEY_END, "End"},
            {KEY_PAGE_UP, "Page Up"},
            {KEY_PAGE_DOWN, "Page Down"},
            {KEY_UP, "Up Arrow"},
            {KEY_DOWN, "Down Arrow"},
            {KEY_LEFT, "Left Arrow"},
            {KEY_RIGHT, "Right Arrow"},
            {KEY_LEFT_SHIFT, "Left Shift"},
            {KEY_RIGHT_SHIFT, "Right Shift"},
            {KEY_LEFT_CONTROL, "Left Ctrl"},
            {KEY_RIGHT_CONTROL, "Right Ctrl"},
            {KEY_LEFT_ALT, "Left Alt"},
            {KEY_RIGHT_ALT, "Right Alt"},
            // Letters
            {KEY_A, "A"},
            {KEY_B, "B"},
            {KEY_C, "C"},
            {KEY_D, "D"},
            {KEY_E, "E"},
            {KEY_F, "F"},
            {KEY_G, "G"},
            {KEY_H, "H"},
            {KEY_I, "I"},
            {KEY_J, "J"},
            {KEY_K, "K"},
            {KEY_L, "L"},
            {KEY_M, "M"},
            {KEY_N, "N"},
            {KEY_O, "O"},
            {KEY_P, "P"},
            {KEY_Q, "Q"},
            {KEY_R, "R"},
            {KEY_S, "S"},
            {KEY_T, "T"},
            {KEY_U, "U"},
            {KEY_V, "V"},
            {KEY_W, "W"},
            {KEY_X, "X"},
            {KEY_Y, "Y"},
            {KEY_Z, "Z"},
            // Numbers
            {KEY_ZERO, "0"},
            {KEY_ONE, "1"},
            {KEY_TWO, "2"},
            {KEY_THREE, "3"},
            {KEY_FOUR, "4"},
            {KEY_FIVE, "5"},
            {KEY_SIX, "6"},
            {KEY_SEVEN, "7"},
            {KEY_EIGHT, "8"},
            {KEY_NINE, "9"},
            // Function keys
            {KEY_F1, "F1"},
            {KEY_F2, "F2"},
            {KEY_F3, "F3"},
            {KEY_F4, "F4"},
            {KEY_F5, "F5"},
            {KEY_F6, "F6"},
            {KEY_F7, "F7"},
            {KEY_F8, "F8"},
            {KEY_F9, "F9"},
            {KEY_F10, "F10"},
            {KEY_F11, "F11"},
            {KEY_F12, "F12"},
            // Numpad
            {KEY_KP_0, "Numpad 0"},
            {KEY_KP_1, "Numpad 1"},
            {KEY_KP_2, "Numpad 2"},
            {KEY_KP_3, "Numpad 3"},
            {KEY_KP_4, "Numpad 4"},
            {KEY_KP_5, "Numpad 5"},
            {KEY_KP_6, "Numpad 6"},
            {KEY_KP_7, "Numpad 7"},
            {KEY_KP_8, "Numpad 8"},
            {KEY_KP_9, "Numpad 9"},
            // Mouse buttons (if needed)
            {KEY_MINUS, "-"},
            {KEY_EQUAL, "="},
            {KEY_LEFT_BRACKET, "["},
            {KEY_RIGHT_BRACKET, "]"},
            {KEY_SEMICOLON, ";"},
            {KEY_APOSTROPHE, "'"},
            {KEY_COMMA, ","},
            {KEY_PERIOD, "."},
            {KEY_SLASH, "/"},
            {KEY_BACKSLASH, "\\"},
            {KEY_GRAVE, "`"},
        };

        auto it = keyNames.find(key);
        if (it != keyNames.end()) {
            return it->second;
        }
        return "Unknown";
    }

    std::string KeyBindings::GetActionName(GameAction action) {
        static const std::unordered_map<GameAction, std::string> actionNames = {
            {GameAction::MOVE_UP, "Move Up"},
            {GameAction::MOVE_DOWN, "Move Down"},
            {GameAction::MOVE_LEFT, "Move Left"},
            {GameAction::MOVE_RIGHT, "Move Right"},
            {GameAction::SHOOT, "Shoot"},
            {GameAction::PAUSE_MENU, "Pause Menu"},
            {GameAction::CHAT_OPEN, "Open Chat"},
            {GameAction::MENU_NEXT, "Menu Next"},
            {GameAction::MENU_PREVIOUS, "Menu Previous"},
            {GameAction::MENU_CONFIRM, "Menu Confirm"},
            {GameAction::MENU_BACK, "Menu Back"},
        };

        auto it = actionNames.find(action);
        if (it != actionNames.end()) {
            return it->second;
        }
        return "Unknown Action";
    }

    void KeyBindings::SetOnBindingsChanged(std::function<void()> callback) {
        _onBindingsChanged = std::move(callback);
    }

    void KeyBindings::NotifyBindingsChanged() {
        if (_onBindingsChanged) {
            _onBindingsChanged();
        }
    }

    bool KeyBindings::SaveToFile(const std::string &filepath) const {
        std::ofstream file(filepath);
        if (!file.is_open()) {
            LOG_ERROR("[KeyBindings] Failed to open file for saving: ", filepath);
            return false;
        }

        for (const auto &[action, binding] : _bindings) {
            file << static_cast<int>(action) << " " << binding.primary << " " << binding.secondary << "\n";
        }

        LOG_INFO("[KeyBindings] Saved bindings to: ", filepath);
        return true;
    }

    bool KeyBindings::LoadFromFile(const std::string &filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            LOG_WARNING("[KeyBindings] No bindings file found, using defaults: ", filepath);
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss(line);
            int actionInt = 0;
            int primary = 0;
            int secondary = 0;
            if (iss >> actionInt >> primary >> secondary) {
                auto action = static_cast<GameAction>(actionInt);
                _bindings[action] = {primary, secondary};
            }
        }

        LOG_INFO("[KeyBindings] Loaded bindings from: ", filepath);
        NotifyBindingsChanged();
        return true;
    }

}  // namespace Input
