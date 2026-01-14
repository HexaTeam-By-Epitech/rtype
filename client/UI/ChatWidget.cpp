/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** ChatWidget implementation
*/

#include "UI/ChatWidget.hpp"
#include <raylib.h>
#include "../common/Logger/Logger.hpp"

namespace Game {

    ChatWidget::ChatWidget(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics)
        : _uiFactory(uiFactory), _graphics(graphics) {}

    void ChatWidget::Initialize() {
        // Create text input for chat
        _textInput = _uiFactory.CreateTextInput();

        if (!_textInput) {
            LOG_ERROR("[ChatWidget] Failed to create text input");
            return;
        }

        // Configure text input
        _textInput->SetMaxLength(MAX_CHAR_PER_MESSAGE);
        _textInput->SetPlaceholder("Type a message...");
        _textInput->SetSize(WIDGET_WIDTH - 2 * PADDING, INPUT_HEIGHT);

        // Colors (dark theme)
        _textInput->SetBackgroundColor(0xFF1A1A1A);    // Dark gray
        _textInput->SetBorderColor(0xFF444444);        // Medium gray
        _textInput->SetActiveBorderColor(0xFF00AAFF);  // Blue when active
        _textInput->SetTextColor(0xFFFFFFFF);          // White
        _textInput->SetPlaceholderColor(0xFF888888);   // Light gray

        // Set callback for when Enter is pressed
        _textInput->SetOnTextChanged([this](const std::string &text) { OnTextChanged(text); });

        LOG_INFO("[ChatWidget] Initialized");
    }

    void ChatWidget::Update() {
        if (!_visible || !_textInput) {
            return;
        }

        _textInput->Update();

        // Check if Enter key is pressed to send message
        if (IsKeyPressed(KEY_ENTER)) {
            SendMessage();
        }
    }

    void ChatWidget::Render() {
        if (!_visible) {
            return;
        }

        // Draw semi-transparent background for chat area
        int bgX = static_cast<int>(_posX);
        int bgY = static_cast<int>(_posY);
        int bgWidth = static_cast<int>(WIDGET_WIDTH);
        int bgHeight = static_cast<int>(WIDGET_HEIGHT);

        _graphics.DrawRectangle(bgX, bgY, bgWidth, bgHeight, 0xAA000000);       // Semi-transparent black
        _graphics.DrawRectangleLines(bgX, bgY, bgWidth, bgHeight, 0xFF444444);  // Gray border

        // Render messages
        float messageY = _posY + PADDING;
        size_t startIdx =
            _messages.size() > MAX_VISIBLE_MESSAGES ? _messages.size() - MAX_VISIBLE_MESSAGES : 0;

        for (size_t i = startIdx; i < _messages.size(); ++i) {
            std::string formatted = FormatMessage(_messages[i]);
            _graphics.DrawText(-1, formatted.c_str(), static_cast<int>(_posX + PADDING),
                               static_cast<int>(messageY), 12, 0xFFFFFFFF);
            messageY += MESSAGE_LINE_HEIGHT + MESSAGE_SPACING;
        }

        // Render text input at bottom
        if (_textInput) {
            _textInput->SetPosition(_posX + PADDING, _posY + WIDGET_HEIGHT - INPUT_HEIGHT - PADDING);
            _textInput->Render();
        }
    }

    void ChatWidget::SetOnMessageSent(std::function<void(const std::string &)> callback) {
        _onMessageSent = callback;
    }

    void ChatWidget::AddMessage(uint32_t playerId, const std::string &playerName, const std::string &message,
                                uint64_t timestamp) {
        _messages.emplace_back(playerId, playerName, message, timestamp);

        // Limit buffer size
        while (_messages.size() > MAX_MESSAGE_BUFFER) {
            _messages.pop_front();
        }

        LOG_DEBUG("[ChatWidget] Added message from ", playerName, ": ", message);
    }

    void ChatWidget::ClearMessages() {
        _messages.clear();
    }

    void ChatWidget::SetVisible(bool visible) {
        _visible = visible;
        if (!visible && _textInput) {
            _textInput->SetText("");  // Clear input when hidden
        }
    }

    void ChatWidget::SetPosition(float x, float y) {
        _posX = x;
        _posY = y;
    }

    void ChatWidget::OnTextChanged(const std::string &text) {
        // Could add real-time validation here if needed
        (void)text;
    }

    void ChatWidget::SendMessage() {
        if (!_textInput) {
            return;
        }

        std::string message = _textInput->GetText();

        // Trim whitespace
        while (!message.empty() && std::isspace(static_cast<unsigned char>(message.front()))) {
            message.erase(message.begin());
        }
        while (!message.empty() && std::isspace(static_cast<unsigned char>(message.back()))) {
            message.pop_back();
        }

        if (message.empty()) {
            return;
        }

        LOG_DEBUG("[ChatWidget] Sending message: '", message,
                  "', callback exists: ", (_onMessageSent ? "YES" : "NO"));

        // Send message via callback
        if (_onMessageSent) {
            LOG_DEBUG("[ChatWidget] Calling onMessageSent callback...");
            _onMessageSent(message);
            LOG_DEBUG("[ChatWidget] Callback completed");
        } else {
            LOG_ERROR("[ChatWidget] No callback registered!");
        }

        // Clear input
        _textInput->SetText("");

        LOG_DEBUG("[ChatWidget] Message sent and input cleared");
    }

    std::string ChatWidget::FormatMessage(const ChatMessageData &msg) {
        // Format: [PlayerName]: Message
        return "[" + msg.playerName + "]: " + msg.message;
    }

}  // namespace Game
