/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ChatWidget - In-game chat component
*/

#pragma once

#include <cstdint>
#include <deque>
#include <functional>
#include <memory>
#include <string>
#include <vector>
#include "Graphics/IGraphics.hpp"
#include "UI/ITextInput.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {

    /**
     * @brief Chat message data
     */
    struct ChatMessageData {
        uint32_t playerId;
        std::string playerName;
        std::string message;
        uint64_t timestamp;

        ChatMessageData(uint32_t id, const std::string &name, const std::string &msg, uint64_t ts)
            : playerId(id), playerName(name), message(msg), timestamp(ts) {}
    };

    /**
     * @brief In-game chat widget
     * 
     * Displays chat messages in bottom-right corner with:
     * - Scrollable message history (limited buffer)
     * - Text input field with character limit
     * - Auto-scroll to latest message
     * - Only visible in waiting room or in-game
     */
    class ChatWidget {
       public:
        explicit ChatWidget(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~ChatWidget() = default;

        /**
         * @brief Initialize the chat widget
         */
        void Initialize();

        /**
         * @brief Update the chat widget
         */
        void Update();

        /**
         * @brief Render the chat widget
         */
        void Render();

        /**
         * @brief Set callback for when a message is sent
         */
        void SetOnMessageSent(std::function<void(const std::string &)> callback);

        /**
         * @brief Add a message to the chat history
         */
        void AddMessage(uint32_t playerId, const std::string &playerName, const std::string &message,
                        uint64_t timestamp);

        /**
         * @brief Clear all messages
         */
        void ClearMessages();

        /**
         * @brief Set widget visibility
         */
        void SetVisible(bool visible);

        /**
         * @brief Check if widget is visible
         */
        bool IsVisible() const { return _visible; }

        /**
         * @brief Set widget position
         */
        void SetPosition(float x, float y);

       private:
        void OnTextChanged(const std::string &text);
        void SendMessage();
        std::string FormatMessage(const ChatMessageData &msg);

        UI::IUIFactory &_uiFactory;
        Graphics::IGraphics &_graphics;

        std::shared_ptr<UI::ITextInput> _textInput;
        std::function<void(const std::string &)> _onMessageSent;

        std::deque<ChatMessageData> _messages;
        bool _visible = false;
        float _posX = 0.0f;
        float _posY = 0.0f;

        // Configuration
        static constexpr size_t MAX_MESSAGE_BUFFER = 50;
        static constexpr size_t MAX_CHAR_PER_MESSAGE = 256;
        static constexpr size_t MAX_VISIBLE_MESSAGES = 6;
        static constexpr float WIDGET_WIDTH = 280.0f;
        static constexpr float WIDGET_HEIGHT = 180.0f;
        static constexpr float INPUT_HEIGHT = 26.0f;
        static constexpr float MESSAGE_SPACING = 3.0f;
        static constexpr float MESSAGE_LINE_HEIGHT = 16.0f;
        static constexpr float PADDING = 8.0f;
    };

}  // namespace Game
