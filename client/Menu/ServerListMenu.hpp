/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ServerListMenu - Menu for selecting a server from saved list
*/

#pragma once

#include <functional>
#include <memory>
#include <string>
#include <vector>

#include "Graphics/IGraphics.hpp"
#include "Menu/BaseMenu.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Server list menu for selecting and managing servers
     *
     * This menu displays a list of saved servers and allows:
     * - Selecting a server to connect to
     * - Adding a new server (opens AddServerMenu)
     * - Going back to main menu
     */
    class ServerListMenu : public BaseMenu {
       public:
        struct ServerInfo {
            std::string name;  ///< Display name (e.g., "Local Server", "EU Server")
            std::string ip;    ///< IP address
            uint16_t port;     ///< Port number

            ServerInfo(const std::string &n, const std::string &i, uint16_t p) : name(n), ip(i), port(p) {}
        };

        explicit ServerListMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~ServerListMenu() override = default;

        /**
         * @brief Set callback triggered when a server is selected
         * @param onServerSelected Callback receiving (ip, port) as parameters
         */
        void SetOnServerSelected(std::function<void(const std::string &, uint16_t)> onServerSelected);

        /**
         * @brief Set callback triggered when "Add Server" button is clicked
         */
        void SetOnAddServer(std::function<void()> onAddServer);

        /**
         * @brief Set callback triggered when the Back button is clicked
         */
        void SetOnBack(std::function<void()> onBack);

        /**
         * @brief Add a server to the list
         * @param name Server display name
         * @param ip Server IP address
         * @param port Server port
         */
        void AddServer(const std::string &name, const std::string &ip, uint16_t port);

        /**
         * @brief Remove a server from the list
         * @param index Index of the server to remove
         */
        void RemoveServer(size_t index);

        /**
         * @brief Get the list of servers
         */
        [[nodiscard]] const std::vector<ServerInfo> &GetServers() const { return _servers; }

        /**
         * @brief Set connection error message
         * @param errorMsg Error message to display
         */
        void SetConnectionError(const std::string &errorMsg);

        /**
         * @brief Clear connection error message
         */
        void ClearConnectionError();

        /**
         * @brief Set connecting state
         * @param connecting True if currently connecting
         * @param serverName Name of server being connected to
         */
        void SetConnecting(bool connecting, const std::string &serverName = "");

        void Initialize() override;
        void Update() override;
        void Render() override;

       private:
        void OnServerClicked(size_t index);
        void OnAddServerClicked();
        void OnBackClicked();
        void OnDeleteServerClicked(size_t index);
        void RebuildServerList();
        void UpdateAddServerButtonState();

        std::function<void(const std::string &, uint16_t)> _onServerSelected{};
        std::function<void()> _onAddServer{};
        std::function<void()> _onBack{};

        Graphics::IGraphics &_graphics;

        std::vector<ServerInfo> _servers;
        std::vector<std::shared_ptr<UI::IButton>> _serverButtons;
        std::vector<std::shared_ptr<UI::IButton>> _deleteButtons;

        std::shared_ptr<UI::IButton> _addServerButton;
        std::shared_ptr<UI::IButton> _backButton;

        std::string _connectionError;
        bool _hasConnectionError{false};

        bool _isConnecting{false};
        std::string _connectingServerName;

        static constexpr size_t MAX_SERVERS = 5;
        static constexpr float SERVER_BUTTON_WIDTH = 400.0f;
        static constexpr float SERVER_BUTTON_HEIGHT = 50.0f;
        static constexpr float SERVER_BUTTON_SPACING = 15.0f;
        static constexpr float LIST_START_Y = 100.0f;
        static constexpr int MAX_VISIBLE_SERVERS = 8;
    };
}  // namespace Game
