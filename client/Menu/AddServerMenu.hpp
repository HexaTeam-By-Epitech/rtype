/*
** EPITECH PROJECT, 2025
** r-type
** File description:
** AddServerMenu - Dialog for adding a new server to the list
*/

#pragma once

#include <functional>
#include <memory>

#include "Graphics/IGraphics.hpp"
#include "Menu/BaseMenu.hpp"
#include "UI/ITextInput.hpp"
#include "UI/IUIFactory.hpp"

namespace Game {
    /**
     * @brief Dialog menu for adding a new server
     *
     * This menu allows the user to:
     * - Enter server name (display name)
     * - Enter server IP address
     * - Enter server port
     * - Confirm or cancel
     */
    class AddServerMenu : public BaseMenu {
       public:
        explicit AddServerMenu(UI::IUIFactory &uiFactory, Graphics::IGraphics &graphics);
        ~AddServerMenu() override = default;

        /**
         * @brief Set callback triggered when the Add button is clicked
         * @param onAdd Callback receiving (name, ip, port) as parameters
         */
        void SetOnAdd(
            std::function<void(const std::string &, const std::string &, const std::string &)> onAdd);

        /**
         * @brief Set callback triggered when the Cancel button is clicked
         */
        void SetOnCancel(std::function<void()> onCancel);

        void Initialize() override;
        void Update() override;
        void Render() override;

       private:
        void OnAddClicked();
        void OnCancelClicked();

        // Helper methods to reduce duplication
        std::unique_ptr<UI::ITextInput> CreateInput(float width, float height, float yPos,
                                                    const std::string &placeholder, int maxLength,
                                                    const std::string &regex, const std::string &logName);
        void ClearInputs();

        // Validation helpers
        bool ValidateName(const std::string &name, std::string &errorMsg);
        bool ValidateIP(const std::string &ip, std::string &errorMsg);
        bool ValidatePort(const std::string &port, std::string &errorMsg);
        void ClearError();

        std::function<void(const std::string &, const std::string &, const std::string &)> _onAdd{};
        std::function<void()> _onCancel{};

        Graphics::IGraphics &_graphics;

        // Text inputs
        std::unique_ptr<UI::ITextInput> _nameInput;
        std::unique_ptr<UI::ITextInput> _ipInput;
        std::unique_ptr<UI::ITextInput> _portInput;

        // Error display
        std::string _errorMessage;
        bool _hasError{false};
        enum class ErrorField { NONE, NAME, IP, PORT };
        ErrorField _errorField{ErrorField::NONE};

        // Button positioning for error message
        float _buttonsY{0.0f};
        float _buttonHeight{0.0f};
    };
}  // namespace Game
