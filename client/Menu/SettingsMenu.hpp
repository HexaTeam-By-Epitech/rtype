/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** SettingsMenu - Settings menu (business logic)
*/

#pragma once

#include <functional>
#include <memory>

#include "Graphics/IGraphics.hpp"
#include "Menu/BaseMenu.hpp"
#include "UI/ISlider.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Settings menu of the game.
     *
     * Business-level class: it uses the UI library but remains decoupled from
     * the graphics backend.
     */
    class SettingsMenu : public BaseMenu {
       public:
        /**
         * @brief Display mode for the settings menu.
         */
        enum class Mode {
            FULLSCREEN,  ///< Takes the whole window (main menu context)
            OVERLAY      ///< Displays over the game with a dimmed background
        };

        /**
         * @brief Construct a new SettingsMenu.
         * @param uiFactory UI factory used to create menus and buttons.
         * @param graphics Graphics interface for rendering slider labels.
         */
        explicit SettingsMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~SettingsMenu() override = default;

        /**
         * @brief Initialize UI elements.
         */
        void Initialize() override;

        void Update() override;
        void Render() override;

        /**
         * @brief Set display mode (fullscreen or overlay).
         */
        void SetMode(Mode mode);
        [[nodiscard]] Mode GetMode() const;

        /**
         * @brief Toggle whether ping should be displayed.
         */
        void SetShowPing(bool enabled);

        /**
         * @brief Toggle auto-matchmaking feature.
         * This triggers the callback to notify the server.
         */
        void SetAutoMatchmaking(bool enabled);

        /**
         * @brief Apply auto-matchmaking preference silently (without triggering callback).
         * Used when loading preference from server after login.
         */
        void ApplyAutoMatchmakingPreference(bool enabled);

        [[nodiscard]] bool GetAutoMatchmaking() const;
        void SetOnAutoMatchmakingChanged(std::function<void(bool)> cb);
        [[nodiscard]] bool GetShowPing() const;

        /**
         * @brief Set callback invoked when the ping toggle changes.
         * @param cb Callback taking the new value.
         */
        void SetOnShowPingChanged(std::function<void(bool)> cb);

        /**
         * @brief Set callback invoked when Back is clicked.
         */
        void SetOnBack(std::function<void()> cb);

        /**
         * @brief Set callback invoked when "Main Menu" is clicked.
         *
         * Intended for the in-game overlay mode.
         */
        void SetOnMainMenu(std::function<void()> cb);

        /**
         * @brief Set the dim color used when this menu is displayed in overlay mode.
         * @param color Color in 0xAARRGGBB.
         */
        void SetOverlayDimColor(unsigned int color);

        /**
         * @brief Get the dim color used when this menu is displayed in overlay mode.
         * @return Color in 0xAARRGGBB.
         */
        [[nodiscard]] unsigned int GetOverlayDimColor() const;

        /**
         * @brief Whether the background should be dimmed when the menu is visible.
         * @return true if mode is OVERLAY and menu is visible.
         */
        [[nodiscard]] bool ShouldDimBackground() const;

        /**
         * @brief Toggle whether FPS should be displayed.
         */
        void SetShowFps(bool enabled);
        [[nodiscard]] bool GetShowFps() const;

        /**
         * @brief Set callback invoked when the FPS toggle changes.
         * @param cb Callback taking the new value.
         */
        void SetOnShowFpsChanged(std::function<void(bool)> cb);

        /**
         * @brief Set the target FPS for the client.
         *
         * Expected values: 30, 60, 120, 144, 240.
         */
        void SetTargetFps(uint32_t targetFps);

        /**
         * @brief Get the target FPS for the client.
         */
        [[nodiscard]] uint32_t GetTargetFps() const;

        /**
         * @brief Set callback invoked when the target FPS changes.
         */
        void SetOnTargetFpsChanged(std::function<void(uint32_t)> callback);

        /**
         * @brief Refresh button labels/colors to match current internal state.
         *
         * Does not rebuild the menu (no Clear/AddButton). Use this when only
         * the visuals need to reflect updated values.
         */
        void RefreshVisuals();

        /**
         * @brief Set showPing without emitting callbacks/logs.
         *
         * Intended for initial state synchronization from the rendering system.
         */
        void SetShowPingSilent(bool enabled);

        /**
         * @brief Set showFps without emitting callbacks/logs.
         */
        void SetShowFpsSilent(bool enabled);

        /**
         * @brief Set targetFps without emitting callbacks/logs.
         */
        void SetTargetFpsSilent(uint32_t targetFps);

        /**
         * @brief Set the volume level (0-100).
         */
        void SetVolume(float volume);

        /**
         * @brief Get the current volume level.
         */
        [[nodiscard]] float GetVolume() const;

        /**
         * @brief Set callback invoked when the volume changes.
         */
        void SetOnVolumeChanged(std::function<void(float)> callback);

        /**
         * @brief Set volume without emitting callbacks/logs.
         */
        void SetVolumeSilent(float volume);

        /**
         * @brief Set chat visibility
         * @param enabled 
         */
        void SetShowChat(bool enabled);

        /**
         * @brief Get chat visibility
         * @return true if chat is shown
         */
        bool GetShowChat() const;

        /**
         * @brief Set callback invoked when the chat visibility changes
         * @param cb Callback invoked when the chat toggle changes.
         */
        void SetOnShowChatChanged(std::function<void(bool)> cb);

        /**
         * @brief Set chat visibility without emitting callbacks/logs.
         * @param enabled enable or disable chat visibility silently
         */
        void SetShowChatSilent(bool enabled);

        /**
         * @brief Updates the visuals of the chat toggle button.
         */
        void UpdateChatToggleVisuals();

       private:
        [[nodiscard]] uint32_t ValidateTargetFps(uint32_t targetFps) const;
        void UpdateToggleVisuals();
        void UpdateFpsToggleVisuals();
        void UpdateAutoMatchmakingVisuals();
        void UpdateTargetFpsVisuals();
        [[nodiscard]] uint32_t NextTargetFps(uint32_t current) const;

        Graphics::IGraphics &_graphics;
        std::unique_ptr<UI::ISlider> _volumeSlider;

        Mode _mode{Mode::FULLSCREEN};
        bool _showPing{true};
        bool _showFps{true};
        bool _showChat{false};         // Désactivé par défaut
        bool _autoMatchmaking{false};  // Auto-matchmaking disabled by default
        unsigned int _overlayDimColor{0x88000000};
        uint32_t _targetFps{60};
        float _volume{50.0F};  // Default volume 50%

        std::function<void(bool)> _onShowPingChanged{};
        std::function<void()> _onBack{};
        std::function<void()> _onMainMenu{};
        std::function<void(bool)> _onShowFpsChanged{};
        std::function<void(bool)> _onShowChatChanged{};
        std::function<void(uint32_t)> _onTargetFpsChanged{};
        std::function<void(float)> _onVolumeChanged{};
        std::function<void(bool)> _onAutoMatchmakingChanged{};

        static constexpr size_t TOGGLE_PING_INDEX = 0;
        static constexpr size_t TOGGLE_FPS_INDEX = 1;
        static constexpr size_t TOGGLE_CHAT_INDEX = 2;
        static constexpr size_t AUTO_MATCHMAKING_INDEX = 3;
        static constexpr size_t TARGET_FPS_INDEX = 4;
        static constexpr size_t BACK_INDEX = 5;
        static constexpr size_t MAIN_MENU_INDEX = 6;
    };
}  // namespace Game
