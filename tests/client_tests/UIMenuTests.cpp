/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** UIMenuTests.cpp - Unit tests for IMenu and menu management
*/

#include <gtest/gtest.h>
#include "Graphics/RaylibGraphics/RaylibGraphics.hpp"
#include "UI/Raylib/RaylibMenu.hpp"
#include "UI/Raylib/RaylibUIFactory.hpp"

// ============================================================================
// Test Fixture for Menu Tests
// ============================================================================
class UIMenuTestFixture : public ::testing::Test {
   protected:
    void SetUp() override {
        graphics = std::make_unique<Graphics::RaylibGraphics>();
        factory = std::make_unique<UI::RaylibUIFactory>(*graphics);
    }

    void TearDown() override {
        factory.reset();
        graphics.reset();
    }

    std::unique_ptr<Graphics::RaylibGraphics> graphics;
    std::unique_ptr<UI::RaylibUIFactory> factory;
};

// ============================================================================
// RaylibMenu Tests
// ============================================================================
TEST_F(UIMenuTestFixture, MenuConstructor) {
    EXPECT_NO_THROW({ UI::RaylibMenu menu(*graphics); });
}

TEST_F(UIMenuTestFixture, MenuVisibilityControl) {
    UI::RaylibMenu menu(*graphics);

    // Should not be visible by default
    EXPECT_FALSE(menu.IsVisible());

    // Show menu
    menu.SetVisible(true);
    EXPECT_TRUE(menu.IsVisible());

    // Hide menu
    menu.SetVisible(false);
    EXPECT_FALSE(menu.IsVisible());
}

TEST_F(UIMenuTestFixture, MenuAddButton) {
    UI::RaylibMenu menu(*graphics);
    auto button = factory->CreateButton();

    EXPECT_NO_THROW({ menu.AddButton(button); });
}

TEST_F(UIMenuTestFixture, MenuAddMultipleButtons) {
    UI::RaylibMenu menu(*graphics);

    auto button1 = factory->CreateButton();
    auto button2 = factory->CreateButton();
    auto button3 = factory->CreateButton();

    button1->SetText("Button 1");
    button2->SetText("Button 2");
    button3->SetText("Button 3");

    EXPECT_NO_THROW({
        menu.AddButton(button1);
        menu.AddButton(button2);
        menu.AddButton(button3);
    });
}

TEST_F(UIMenuTestFixture, MenuGetButton) {
    UI::RaylibMenu menu(*graphics);

    auto button1 = factory->CreateButton();
    auto button2 = factory->CreateButton();

    button1->SetText("First");
    button2->SetText("Second");

    menu.AddButton(button1);
    menu.AddButton(button2);

    // Get buttons by index
    auto retrievedButton1 = menu.GetButton(0);
    auto retrievedButton2 = menu.GetButton(1);

    EXPECT_NE(retrievedButton1, nullptr);
    EXPECT_NE(retrievedButton2, nullptr);
}

TEST_F(UIMenuTestFixture, MenuGetButtonOutOfRange) {
    UI::RaylibMenu menu(*graphics);

    auto button = factory->CreateButton();
    menu.AddButton(button);

    // Try to get button at invalid index
    auto invalidButton = menu.GetButton(999);
    EXPECT_EQ(invalidButton, nullptr);
}

TEST_F(UIMenuTestFixture, MenuClear) {
    UI::RaylibMenu menu(*graphics);

    auto button1 = factory->CreateButton();
    auto button2 = factory->CreateButton();

    menu.AddButton(button1);
    menu.AddButton(button2);

    // Clear menu
    EXPECT_NO_THROW({ menu.Clear(); });

    // Buttons should no longer be accessible
    auto button = menu.GetButton(0);
    EXPECT_EQ(button, nullptr);
}

TEST_F(UIMenuTestFixture, MenuUpdateWithNoButtons) {
    UI::RaylibMenu menu(*graphics);

    // Should not crash when updating with no buttons
    EXPECT_NO_THROW({ menu.Update(); });
}

TEST_F(UIMenuTestFixture, MenuUpdateWithButtons) {
    UI::RaylibMenu menu(*graphics);

    auto button = factory->CreateButton();
    menu.AddButton(button);

    EXPECT_NO_THROW({ menu.Update(); });
}

TEST_F(UIMenuTestFixture, MenuRenderWhenInvisible) {
    UI::RaylibMenu menu(*graphics);
    menu.SetVisible(false);

    // Should not crash when rendering invisible menu
    EXPECT_NO_THROW({ menu.Render(); });
}

TEST_F(UIMenuTestFixture, MenuRenderWhenVisible) {
    UI::RaylibMenu menu(*graphics);

    auto button = factory->CreateButton();
    button->SetText("Test");
    menu.AddButton(button);

    menu.SetVisible(true);

    EXPECT_NO_THROW({ menu.Render(); });
}

// ============================================================================
// Menu Button Interaction Tests
// ============================================================================
TEST_F(UIMenuTestFixture, MenuButtonCallbackIntegration) {
    UI::RaylibMenu menu(*graphics);

    bool button1Clicked = false;
    bool button2Clicked = false;

    auto button1 = factory->CreateButton();
    auto button2 = factory->CreateButton();

    button1->SetCallback([&button1Clicked]() { button1Clicked = true; });
    button2->SetCallback([&button2Clicked]() { button2Clicked = true; });

    menu.AddButton(button1);
    menu.AddButton(button2);

    // Callbacks should be set but not called yet
    EXPECT_FALSE(button1Clicked);
    EXPECT_FALSE(button2Clicked);
}

TEST_F(UIMenuTestFixture, MenuButtonLayout) {
    UI::RaylibMenu menu(*graphics);

    const float buttonWidth = 200.0f;
    const float buttonHeight = 50.0f;
    const float spacing = 10.0f;

    // Create buttons with specific layout
    for (int i = 0; i < 3; i++) {
        auto button = factory->CreateButton();
        button->SetSize(buttonWidth, buttonHeight);
        button->SetPosition(100.0f, 100.0f + (buttonHeight + spacing) * i);
        button->SetText("Button " + std::to_string(i + 1));
        menu.AddButton(button);
    }

    // Verify buttons are accessible
    for (size_t i = 0; i < 3; i++) {
        auto button = menu.GetButton(i);
        EXPECT_NE(button, nullptr);

        float x, y;
        button->GetPosition(x, y);
        EXPECT_FLOAT_EQ(x, 100.0f);
        EXPECT_FLOAT_EQ(y, 100.0f + (buttonHeight + spacing) * i);
    }
}

TEST_F(UIMenuTestFixture, MenuDynamicButtonAddRemove) {
    UI::RaylibMenu menu(*graphics);

    // Add initial buttons
    auto button1 = factory->CreateButton();
    auto button2 = factory->CreateButton();
    menu.AddButton(button1);
    menu.AddButton(button2);

    EXPECT_NE(menu.GetButton(0), nullptr);
    EXPECT_NE(menu.GetButton(1), nullptr);

    // Clear and add new buttons
    menu.Clear();

    auto button3 = factory->CreateButton();
    menu.AddButton(button3);

    EXPECT_NE(menu.GetButton(0), nullptr);
    EXPECT_EQ(menu.GetButton(1), nullptr);  // Should not exist after clear
}

// ============================================================================
// Menu State Management Tests
// ============================================================================
TEST_F(UIMenuTestFixture, MenuMultipleShowHideCycles) {
    UI::RaylibMenu menu(*graphics);

    for (int i = 0; i < 5; i++) {
        menu.SetVisible(true);
        EXPECT_TRUE(menu.IsVisible());

        menu.SetVisible(false);
        EXPECT_FALSE(menu.IsVisible());
    }
}

TEST_F(UIMenuTestFixture, MenuUpdateRenderCycle) {
    UI::RaylibMenu menu(*graphics);
    menu.SetVisible(true);

    auto button = factory->CreateButton();
    button->SetText("Cycle Test");
    menu.AddButton(button);

    // Simulate multiple update-render cycles
    for (int i = 0; i < 10; i++) {
        EXPECT_NO_THROW({
            menu.Update();
            menu.Render();
        });
    }
}

// ============================================================================
// Menu Factory Integration Tests
// ============================================================================
TEST_F(UIMenuTestFixture, FactoryCreateMenuAndPopulate) {
    auto menu = factory->CreateMenu();
    EXPECT_NE(menu, nullptr);

    // Populate with buttons
    for (int i = 0; i < 5; i++) {
        auto button = factory->CreateButton();
        button->SetText("Menu Button " + std::to_string(i + 1));
        button->SetSize(200.0f, 50.0f);
        menu->AddButton(button);
    }

    // Verify all buttons are accessible
    for (size_t i = 0; i < 5; i++) {
        auto button = menu->GetButton(i);
        EXPECT_NE(button, nullptr);
    }
}

TEST_F(UIMenuTestFixture, FactoryCreateMultipleMenus) {
    auto menu1 = factory->CreateMenu();
    auto menu2 = factory->CreateMenu();
    auto menu3 = factory->CreateMenu();

    EXPECT_NE(menu1, nullptr);
    EXPECT_NE(menu2, nullptr);
    EXPECT_NE(menu3, nullptr);

    // Each menu should be independent
    menu1->SetVisible(true);
    menu2->SetVisible(false);

    EXPECT_TRUE(menu1->IsVisible());
    EXPECT_FALSE(menu2->IsVisible());
}

// ============================================================================
// Complex Menu Scenarios
// ============================================================================
TEST_F(UIMenuTestFixture, MenuWithMixedComponents) {
    UI::RaylibMenu menu(*graphics);

    // Add different types of buttons
    auto playButton = factory->CreateButton();
    playButton->SetText("PLAY");
    playButton->SetBackgroundColor(0xFF4CAF50);

    auto settingsButton = factory->CreateButton();
    settingsButton->SetText("SETTINGS");
    settingsButton->SetBackgroundColor(0xFF424242);

    auto quitButton = factory->CreateButton();
    quitButton->SetText("QUIT");
    quitButton->SetBackgroundColor(0xFFF44336);

    menu.AddButton(playButton);
    menu.AddButton(settingsButton);
    menu.AddButton(quitButton);

    menu.SetVisible(true);

    // Simulate game loop
    EXPECT_NO_THROW({
        for (int frame = 0; frame < 60; frame++) {
            menu.Update();
            menu.Render();
        }
    });
}

TEST_F(UIMenuTestFixture, MenuNavigationPattern) {
    // Simulate menu stack navigation
    auto mainMenu = factory->CreateMenu();
    auto subMenu = factory->CreateMenu();

    mainMenu->SetVisible(true);
    subMenu->SetVisible(false);

    EXPECT_TRUE(mainMenu->IsVisible());
    EXPECT_FALSE(subMenu->IsVisible());

    // Navigate to sub-menu
    mainMenu->SetVisible(false);
    subMenu->SetVisible(true);

    EXPECT_FALSE(mainMenu->IsVisible());
    EXPECT_TRUE(subMenu->IsVisible());

    // Navigate back
    subMenu->SetVisible(false);
    mainMenu->SetVisible(true);

    EXPECT_TRUE(mainMenu->IsVisible());
    EXPECT_FALSE(subMenu->IsVisible());
}
