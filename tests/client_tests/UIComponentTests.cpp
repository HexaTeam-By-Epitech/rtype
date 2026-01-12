/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** UIComponentTests.cpp - Unit tests for UI components (Button, TextInput, Slider)
*/

#include <gtest/gtest.h>
#include "Graphics/RaylibGraphics/RaylibGraphics.hpp"
#include "UI/Raylib/RaylibButton.hpp"
#include "UI/Raylib/RaylibSlider.hpp"
#include "UI/Raylib/RaylibTextInput.hpp"
#include "UI/Raylib/RaylibUIFactory.hpp"

// ============================================================================
// Test Fixture for UI Component Tests
// ============================================================================
class UIComponentTestFixture : public ::testing::Test {
   protected:
    void SetUp() override { graphics = std::make_unique<Graphics::RaylibGraphics>(); }

    void TearDown() override { graphics.reset(); }

    std::unique_ptr<Graphics::RaylibGraphics> graphics;
};

// ============================================================================
// RaylibButton Tests
// ============================================================================
TEST_F(UIComponentTestFixture, ButtonConstructor) {
    EXPECT_NO_THROW({ UI::RaylibButton button(*graphics); });
}

TEST_F(UIComponentTestFixture, ButtonSetAndGetPosition) {
    UI::RaylibButton button(*graphics);

    button.SetPosition(100.0f, 200.0f);

    float x, y;
    button.GetPosition(x, y);

    EXPECT_FLOAT_EQ(x, 100.0f);
    EXPECT_FLOAT_EQ(y, 200.0f);
}

TEST_F(UIComponentTestFixture, ButtonSetAndGetSize) {
    UI::RaylibButton button(*graphics);

    button.SetSize(150.0f, 50.0f);

    float width, height;
    button.GetSize(width, height);

    EXPECT_FLOAT_EQ(width, 150.0f);
    EXPECT_FLOAT_EQ(height, 50.0f);
}

TEST_F(UIComponentTestFixture, ButtonSetText) {
    UI::RaylibButton button(*graphics);

    EXPECT_NO_THROW(button.SetText("Test Button"));
}

TEST_F(UIComponentTestFixture, ButtonSetColors) {
    UI::RaylibButton button(*graphics);

    EXPECT_NO_THROW({
        button.SetBackgroundColor(0xFF000000);
        button.SetHoverColor(0xFFFF0000);
        button.SetTextColor(0xFFFFFFFF);
    });
}

TEST_F(UIComponentTestFixture, ButtonCallback) {
    UI::RaylibButton button(*graphics);

    bool callbackCalled = false;
    button.SetCallback([&callbackCalled]() { callbackCalled = true; });

    EXPECT_FALSE(callbackCalled);
    // Note: Actually triggering the callback would require simulating mouse input
}

TEST_F(UIComponentTestFixture, ButtonEnableDisable) {
    UI::RaylibButton button(*graphics);

    // Should be enabled by default
    EXPECT_TRUE(button.IsEnabled());

    button.SetEnabled(false);
    EXPECT_FALSE(button.IsEnabled());

    button.SetEnabled(true);
    EXPECT_TRUE(button.IsEnabled());
}

TEST_F(UIComponentTestFixture, ButtonAlignment) {
    UI::RaylibButton button(*graphics);

    EXPECT_NO_THROW({
        button.SetAlign(UI::Align::CENTER_BOTH);
        EXPECT_EQ(button.GetAlign(), UI::Align::CENTER_BOTH);

        button.SetAlign(UI::Align::CENTER_HORIZONTAL);
        EXPECT_EQ(button.GetAlign(), UI::Align::CENTER_HORIZONTAL);

        button.SetAlign(UI::Align::CENTER_VERTICAL);
        EXPECT_EQ(button.GetAlign(), UI::Align::CENTER_VERTICAL);

        button.SetAlign(UI::Align::NONE);
        EXPECT_EQ(button.GetAlign(), UI::Align::NONE);
    });
}

// ============================================================================
// RaylibTextInput Tests
// ============================================================================
TEST_F(UIComponentTestFixture, TextInputConstructor) {
    EXPECT_NO_THROW({ UI::RaylibTextInput input(*graphics); });
}

TEST_F(UIComponentTestFixture, TextInputSetAndGetText) {
    UI::RaylibTextInput input(*graphics);

    input.SetText("Hello World");
    EXPECT_EQ(input.GetText(), "Hello World");
}

TEST_F(UIComponentTestFixture, TextInputClear) {
    UI::RaylibTextInput input(*graphics);

    input.SetText("Test");
    EXPECT_FALSE(input.GetText().empty());

    input.Clear();
    EXPECT_TRUE(input.GetText().empty());
}

TEST_F(UIComponentTestFixture, TextInputMaxLength) {
    UI::RaylibTextInput input(*graphics);

    input.SetMaxLength(5);
    input.SetText("12345678");  // Exceeds max length

    // Should be truncated to 5 characters
    EXPECT_EQ(input.GetText().length(), 5);
    EXPECT_EQ(input.GetText(), "12345");
}

TEST_F(UIComponentTestFixture, TextInputPlaceholder) {
    UI::RaylibTextInput input(*graphics);

    EXPECT_NO_THROW({ input.SetPlaceholder("Enter text..."); });
}

TEST_F(UIComponentTestFixture, TextInputFocusControl) {
    UI::RaylibTextInput input(*graphics);

    // Should not be focused by default
    EXPECT_FALSE(input.IsFocused());

    input.SetFocused(true);
    EXPECT_TRUE(input.IsFocused());

    input.SetFocused(false);
    EXPECT_FALSE(input.IsFocused());
}

TEST_F(UIComponentTestFixture, TextInputEnableDisable) {
    UI::RaylibTextInput input(*graphics);

    EXPECT_TRUE(input.IsEnabled());

    input.SetEnabled(false);
    EXPECT_FALSE(input.IsEnabled());
    EXPECT_FALSE(input.IsFocused());  // Should lose focus when disabled

    input.SetEnabled(true);
    EXPECT_TRUE(input.IsEnabled());
}

TEST_F(UIComponentTestFixture, TextInputValidationRegex) {
    UI::RaylibTextInput input(*graphics);

    // Set validation for digits only
    EXPECT_NO_THROW({ input.SetValidationRegex("[0-9]+"); });
}

TEST_F(UIComponentTestFixture, TextInputColors) {
    UI::RaylibTextInput input(*graphics);

    EXPECT_NO_THROW({
        input.SetBackgroundColor(0xFF000000);
        input.SetBorderColor(0xFF555555);
        input.SetActiveBorderColor(0xFF00FF00);
        input.SetTextColor(0xFFFFFFFF);
        input.SetPlaceholderColor(0xFF888888);
    });
}

TEST_F(UIComponentTestFixture, TextInputCallback) {
    UI::RaylibTextInput input(*graphics);

    std::string capturedText;
    input.SetOnTextChanged([&capturedText](const std::string &text) { capturedText = text; });

    // Note: Callback would be triggered on actual text input
}

TEST_F(UIComponentTestFixture, TextInputSetAndGetPosition) {
    UI::RaylibTextInput input(*graphics);

    input.SetPosition(50.0f, 75.0f);

    float x, y;
    input.GetPosition(x, y);

    EXPECT_FLOAT_EQ(x, 50.0f);
    EXPECT_FLOAT_EQ(y, 75.0f);
}

TEST_F(UIComponentTestFixture, TextInputSetAndGetSize) {
    UI::RaylibTextInput input(*graphics);

    input.SetSize(300.0f, 40.0f);

    float width, height;
    input.GetSize(width, height);

    EXPECT_FLOAT_EQ(width, 300.0f);
    EXPECT_FLOAT_EQ(height, 40.0f);
}

// ============================================================================
// RaylibSlider Tests
// ============================================================================
TEST_F(UIComponentTestFixture, SliderConstructor) {
    EXPECT_NO_THROW({ UI::RaylibSlider slider(*graphics); });
}

TEST_F(UIComponentTestFixture, SliderSetAndGetValue) {
    UI::RaylibSlider slider(*graphics);

    slider.SetMinValue(0.0f);
    slider.SetMaxValue(100.0f);
    slider.SetValue(50.0f);

    EXPECT_FLOAT_EQ(slider.GetValue(), 50.0f);
}

TEST_F(UIComponentTestFixture, SliderValueClamping) {
    UI::RaylibSlider slider(*graphics);

    slider.SetMinValue(0.0f);
    slider.SetMaxValue(100.0f);

    // Test value below minimum
    slider.SetValue(-10.0f);
    EXPECT_GE(slider.GetValue(), 0.0f);

    // Test value above maximum
    slider.SetValue(150.0f);
    EXPECT_LE(slider.GetValue(), 100.0f);
}

TEST_F(UIComponentTestFixture, SliderMinMaxValues) {
    UI::RaylibSlider slider(*graphics);

    slider.SetMinValue(10.0f);
    slider.SetMaxValue(90.0f);

    EXPECT_FLOAT_EQ(slider.GetMinValue(), 10.0f);
    EXPECT_FLOAT_EQ(slider.GetMaxValue(), 90.0f);
}

TEST_F(UIComponentTestFixture, SliderColors) {
    UI::RaylibSlider slider(*graphics);

    EXPECT_NO_THROW({
        slider.SetTrackColor(0xFF333333);
        slider.SetFilledColor(0xFF00FF00);
        slider.SetHandleColor(0xFFFFFFFF);
        slider.SetHandleHoverColor(0xFFCCCCCC);
    });
}

TEST_F(UIComponentTestFixture, SliderCallback) {
    UI::RaylibSlider slider(*graphics);

    float capturedValue = 0.0f;
    slider.SetOnValueChanged([&capturedValue](float value) { capturedValue = value; });

    // Note: Callback would be triggered on actual slider interaction
}

TEST_F(UIComponentTestFixture, SliderEnableDisable) {
    UI::RaylibSlider slider(*graphics);

    EXPECT_TRUE(slider.IsEnabled());

    slider.SetEnabled(false);
    EXPECT_FALSE(slider.IsEnabled());

    slider.SetEnabled(true);
    EXPECT_TRUE(slider.IsEnabled());
}

TEST_F(UIComponentTestFixture, SliderSetAndGetPosition) {
    UI::RaylibSlider slider(*graphics);

    slider.SetPosition(100.0f, 150.0f);

    float x, y;
    slider.GetPosition(x, y);

    EXPECT_FLOAT_EQ(x, 100.0f);
    EXPECT_FLOAT_EQ(y, 150.0f);
}

TEST_F(UIComponentTestFixture, SliderSetAndGetSize) {
    UI::RaylibSlider slider(*graphics);

    slider.SetSize(250.0f, 20.0f);

    float width, height;
    slider.GetSize(width, height);

    EXPECT_FLOAT_EQ(width, 250.0f);
    EXPECT_FLOAT_EQ(height, 20.0f);
}

// ============================================================================
// RaylibUIFactory Tests
// ============================================================================
TEST_F(UIComponentTestFixture, UIFactoryCreateButton) {
    UI::RaylibUIFactory factory(*graphics);

    auto button = factory.CreateButton();
    EXPECT_NE(button, nullptr);
}

TEST_F(UIComponentTestFixture, UIFactoryCreateTextInput) {
    UI::RaylibUIFactory factory(*graphics);

    auto textInput = factory.CreateTextInput();
    EXPECT_NE(textInput, nullptr);
}

TEST_F(UIComponentTestFixture, UIFactoryCreateSlider) {
    UI::RaylibUIFactory factory(*graphics);

    auto slider = factory.CreateSlider();
    EXPECT_NE(slider, nullptr);
}

TEST_F(UIComponentTestFixture, UIFactoryCreateMenu) {
    UI::RaylibUIFactory factory(*graphics);

    auto menu = factory.CreateMenu();
    EXPECT_NE(menu, nullptr);
}

TEST_F(UIComponentTestFixture, UIFactoryCreateMultipleComponents) {
    UI::RaylibUIFactory factory(*graphics);

    auto button1 = factory.CreateButton();
    auto button2 = factory.CreateButton();
    auto input = factory.CreateTextInput();
    auto slider = factory.CreateSlider();
    auto menu = factory.CreateMenu();

    EXPECT_NE(button1, nullptr);
    EXPECT_NE(button2, nullptr);
    EXPECT_NE(input, nullptr);
    EXPECT_NE(slider, nullptr);
    EXPECT_NE(menu, nullptr);

    // Each component should be independent
    EXPECT_NE(button1.get(), button2.get());
}

// ============================================================================
// Integration Tests
// ============================================================================
TEST_F(UIComponentTestFixture, ButtonTextInputInteractionPattern) {
    UI::RaylibButton submitButton(*graphics);
    UI::RaylibTextInput textInput(*graphics);

    // Setup
    textInput.SetPosition(100.0f, 100.0f);
    textInput.SetSize(200.0f, 40.0f);

    submitButton.SetPosition(100.0f, 150.0f);
    submitButton.SetSize(200.0f, 40.0f);
    submitButton.SetText("Submit");

    std::string submittedText;
    bool submitted = false;

    submitButton.SetCallback([&]() {
        submittedText = textInput.GetText();
        submitted = true;
    });

    // Simulate user entering text
    textInput.SetText("Test Input");

    // Verify state before submission
    EXPECT_FALSE(submitted);
    EXPECT_TRUE(submittedText.empty());
    EXPECT_EQ(textInput.GetText(), "Test Input");
}

TEST_F(UIComponentTestFixture, SliderWithValueDisplay) {
    UI::RaylibSlider slider(*graphics);

    slider.SetMinValue(0.0f);
    slider.SetMaxValue(100.0f);
    slider.SetValue(75.0f);

    float displayedValue = slider.GetValue();
    EXPECT_FLOAT_EQ(displayedValue, 75.0f);

    // Simulate slider change
    slider.SetValue(25.0f);
    displayedValue = slider.GetValue();
    EXPECT_FLOAT_EQ(displayedValue, 25.0f);
}

TEST_F(UIComponentTestFixture, FormValidationPattern) {
    UI::RaylibTextInput nicknameInput(*graphics);
    UI::RaylibTextInput ipInput(*graphics);
    UI::RaylibButton submitButton(*graphics);

    // Setup validation
    nicknameInput.SetValidationRegex("[a-zA-Z0-9_-]+");
    nicknameInput.SetMaxLength(20);

    ipInput.SetValidationRegex("[0-9.]+");
    ipInput.SetMaxLength(15);

    // Simulate valid input
    nicknameInput.SetText("Player123");
    ipInput.SetText("192.168.1.1");

    EXPECT_EQ(nicknameInput.GetText(), "Player123");
    EXPECT_EQ(ipInput.GetText(), "192.168.1.1");
}
