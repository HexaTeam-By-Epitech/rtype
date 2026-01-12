/*
** EPITECH PROJECT, 2026
** r-type
** File description:
** MenuTests.cpp - Unit tests for Menu classes
*/

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include "Graphics/RaylibGraphics/RaylibGraphics.hpp"
#include "Menu/BaseMenu.hpp"
#include "Menu/ConfirmQuitMenu.hpp"
#include "Menu/ConnectionMenu.hpp"
#include "Menu/MainMenu.hpp"
#include "Menu/SettingsMenu.hpp"
#include "UI/Raylib/RaylibUIFactory.hpp"

using ::testing::_;
using ::testing::Return;

// ============================================================================
// Test Fixture for Menu Tests
// ============================================================================
class MenuTestFixture : public ::testing::Test {
   protected:
    void SetUp() override {
        // Initialize graphics for UI factory
        graphics = std::make_unique<Graphics::RaylibGraphics>();
        uiFactory = std::make_unique<UI::RaylibUIFactory>(*graphics);
    }

    void TearDown() override {
        graphics.reset();
        uiFactory.reset();
    }

    std::unique_ptr<Graphics::RaylibGraphics> graphics;
    std::unique_ptr<UI::RaylibUIFactory> uiFactory;
};

// ============================================================================
// BaseMenu Tests
// ============================================================================
TEST_F(MenuTestFixture, BaseMenuConstructorCreatesMenu) {
    // Create a concrete implementation for testing
    class TestMenu : public Game::BaseMenu {
       public:
        explicit TestMenu(UI::IUIFactory &factory) : BaseMenu(factory) {}
        void Initialize() override {
            if (_menu) {
                _menu->Clear();
            }
        }
    };

    TestMenu menu(*uiFactory);
    EXPECT_NO_THROW(menu.Initialize());
}

TEST_F(MenuTestFixture, BaseMenuVisibilityControl) {
    class TestMenu : public Game::BaseMenu {
       public:
        explicit TestMenu(UI::IUIFactory &factory) : BaseMenu(factory) {}
        void Initialize() override {}
    };

    TestMenu menu(*uiFactory);

    // Initially should not be visible
    EXPECT_FALSE(menu.IsVisible());

    // Show menu
    menu.Show();
    EXPECT_TRUE(menu.IsVisible());

    // Hide menu
    menu.Hide();
    EXPECT_FALSE(menu.IsVisible());
}

// ============================================================================
// MainMenu Tests
// ============================================================================
TEST_F(MenuTestFixture, MainMenuInitialization) {
    Game::MainMenu menu(*uiFactory);
    EXPECT_NO_THROW(menu.Initialize());
}

TEST_F(MenuTestFixture, MainMenuCallbacksAreSet) {
    Game::MainMenu menu(*uiFactory);

    bool playClicked = false;
    bool settingsClicked = false;
    bool quitClicked = false;

    menu.SetOnPlay([&playClicked]() { playClicked = true; });
    menu.SetOnSettings([&settingsClicked]() { settingsClicked = true; });
    menu.SetOnQuit([&quitClicked]() { quitClicked = true; });

    menu.Initialize();

    // Callbacks should be set without errors
    EXPECT_FALSE(playClicked);
    EXPECT_FALSE(settingsClicked);
    EXPECT_FALSE(quitClicked);
}

TEST_F(MenuTestFixture, MainMenuVisibilityToggle) {
    Game::MainMenu menu(*uiFactory);
    menu.Initialize();

    EXPECT_FALSE(menu.IsVisible());

    menu.Show();
    EXPECT_TRUE(menu.IsVisible());

    menu.Hide();
    EXPECT_FALSE(menu.IsVisible());
}

// ============================================================================
// ConfirmQuitMenu Tests
// ============================================================================
TEST_F(MenuTestFixture, ConfirmQuitMenuInitialization) {
    Game::ConfirmQuitMenu menu(*uiFactory);
    EXPECT_NO_THROW(menu.Initialize());
}

TEST_F(MenuTestFixture, ConfirmQuitMenuCallbacks) {
    Game::ConfirmQuitMenu menu(*uiFactory);

    bool confirmCalled = false;
    bool cancelCalled = false;

    menu.SetOnConfirm([&confirmCalled]() { confirmCalled = true; });
    menu.SetOnCancel([&cancelCalled]() { cancelCalled = true; });

    menu.Initialize();

    // Callbacks should be registered
    EXPECT_FALSE(confirmCalled);
    EXPECT_FALSE(cancelCalled);
}

TEST_F(MenuTestFixture, ConfirmQuitMenuVisibilityControl) {
    Game::ConfirmQuitMenu menu(*uiFactory);
    menu.Initialize();

    EXPECT_FALSE(menu.IsVisible());

    menu.Show();
    EXPECT_TRUE(menu.IsVisible());

    menu.Hide();
    EXPECT_FALSE(menu.IsVisible());
}

// ============================================================================
// SettingsMenu Tests
// ============================================================================
TEST_F(MenuTestFixture, SettingsMenuInitialization) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    EXPECT_NO_THROW(menu.Initialize());
}

TEST_F(MenuTestFixture, SettingsMenuModeToggle) {
    Game::SettingsMenu menu(*uiFactory, *graphics);

    // Default should be FULLSCREEN
    EXPECT_EQ(menu.GetMode(), Game::SettingsMenu::Mode::FULLSCREEN);

    // Switch to OVERLAY
    menu.SetMode(Game::SettingsMenu::Mode::OVERLAY);
    EXPECT_EQ(menu.GetMode(), Game::SettingsMenu::Mode::OVERLAY);

    // Switch back to FULLSCREEN
    menu.SetMode(Game::SettingsMenu::Mode::FULLSCREEN);
    EXPECT_EQ(menu.GetMode(), Game::SettingsMenu::Mode::FULLSCREEN);
}

TEST_F(MenuTestFixture, SettingsMenuShowPingToggle) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    // Default should be true
    EXPECT_TRUE(menu.GetShowPing());

    bool callbackTriggered = false;
    menu.SetOnShowPingChanged([&callbackTriggered](bool enabled) { callbackTriggered = true; });

    // Toggle off
    menu.SetShowPing(false);
    EXPECT_FALSE(menu.GetShowPing());
    EXPECT_TRUE(callbackTriggered);

    // Toggle on
    callbackTriggered = false;
    menu.SetShowPing(true);
    EXPECT_TRUE(menu.GetShowPing());
    EXPECT_TRUE(callbackTriggered);
}

TEST_F(MenuTestFixture, SettingsMenuShowFpsToggle) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    // Default should be true
    EXPECT_TRUE(menu.GetShowFps());

    bool callbackTriggered = false;
    menu.SetOnShowFpsChanged([&callbackTriggered](bool enabled) { callbackTriggered = true; });

    // Toggle off
    menu.SetShowFps(false);
    EXPECT_FALSE(menu.GetShowFps());
    EXPECT_TRUE(callbackTriggered);
}

TEST_F(MenuTestFixture, SettingsMenuTargetFpsValidation) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    // Default should be 60
    EXPECT_EQ(menu.GetTargetFps(), 60);

    // Set valid FPS values
    menu.SetTargetFps(30);
    EXPECT_EQ(menu.GetTargetFps(), 30);

    menu.SetTargetFps(120);
    EXPECT_EQ(menu.GetTargetFps(), 120);

    menu.SetTargetFps(144);
    EXPECT_EQ(menu.GetTargetFps(), 144);

    menu.SetTargetFps(240);
    EXPECT_EQ(menu.GetTargetFps(), 240);
}

TEST_F(MenuTestFixture, SettingsMenuVolumeControl) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    // Default should be 50%
    EXPECT_FLOAT_EQ(menu.GetVolume(), 50.0f);

    // Set volume to 75%
    menu.SetVolume(75.0f);
    EXPECT_FLOAT_EQ(menu.GetVolume(), 75.0f);

    // Test bounds (0-100)
    menu.SetVolume(0.0f);
    EXPECT_FLOAT_EQ(menu.GetVolume(), 0.0f);

    menu.SetVolume(100.0f);
    EXPECT_FLOAT_EQ(menu.GetVolume(), 100.0f);

    // Test clamping beyond bounds
    menu.SetVolume(-10.0f);
    EXPECT_GE(menu.GetVolume(), 0.0f);

    menu.SetVolume(150.0f);
    EXPECT_LE(menu.GetVolume(), 100.0f);
}

TEST_F(MenuTestFixture, SettingsMenuSilentSetters) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    bool callbackCalled = false;
    menu.SetOnShowPingChanged([&callbackCalled](bool) { callbackCalled = true; });

    // Silent setter should not trigger callback
    menu.SetShowPingSilent(false);
    EXPECT_FALSE(menu.GetShowPing());
    EXPECT_FALSE(callbackCalled);

    // Normal setter should trigger callback
    menu.SetShowPing(true);
    EXPECT_TRUE(callbackCalled);
}

TEST_F(MenuTestFixture, SettingsMenuOverlayDimColor) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    // Test default dim color
    unsigned int defaultColor = menu.GetOverlayDimColor();
    EXPECT_EQ(defaultColor, 0x88000000);

    // Set custom dim color
    menu.SetOverlayDimColor(0xAA333333);
    EXPECT_EQ(menu.GetOverlayDimColor(), 0xAA333333);
}

TEST_F(MenuTestFixture, SettingsMenuShouldDimBackground) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    // Should not dim in FULLSCREEN mode
    menu.SetMode(Game::SettingsMenu::Mode::FULLSCREEN);
    menu.Show();
    EXPECT_FALSE(menu.ShouldDimBackground());

    // Should dim in OVERLAY mode when visible
    menu.SetMode(Game::SettingsMenu::Mode::OVERLAY);
    EXPECT_TRUE(menu.ShouldDimBackground());

    // Should not dim when hidden
    menu.Hide();
    EXPECT_FALSE(menu.ShouldDimBackground());
}

// ============================================================================
// ConnectionMenu Tests
// ============================================================================
TEST_F(MenuTestFixture, ConnectionMenuInitialization) {
    Game::ConnectionMenu menu(*uiFactory, *graphics);
    EXPECT_NO_THROW(menu.Initialize());
}

TEST_F(MenuTestFixture, ConnectionMenuCallbacks) {
    Game::ConnectionMenu menu(*uiFactory, *graphics);

    bool joinCalled = false;
    bool backCalled = false;
    std::string capturedNickname, capturedIp, capturedPort;

    menu.SetOnJoin([&](const std::string &nick, const std::string &ip, const std::string &port) {
        joinCalled = true;
        capturedNickname = nick;
        capturedIp = ip;
        capturedPort = port;
    });

    menu.SetOnBack([&backCalled]() { backCalled = true; });

    menu.Initialize();

    // Callbacks should be registered
    EXPECT_FALSE(joinCalled);
    EXPECT_FALSE(backCalled);
}

TEST_F(MenuTestFixture, ConnectionMenuVisibilityControl) {
    Game::ConnectionMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    EXPECT_FALSE(menu.IsVisible());

    menu.Show();
    EXPECT_TRUE(menu.IsVisible());

    menu.Hide();
    EXPECT_FALSE(menu.IsVisible());
}

// ============================================================================
// Integration Tests
// ============================================================================
TEST_F(MenuTestFixture, MenuTransitionFlow) {
    Game::MainMenu mainMenu(*uiFactory);
    Game::ConnectionMenu connectionMenu(*uiFactory, *graphics);
    Game::SettingsMenu settingsMenu(*uiFactory, *graphics);

    mainMenu.Initialize();
    connectionMenu.Initialize();
    settingsMenu.Initialize();

    // Start with main menu visible
    mainMenu.Show();
    EXPECT_TRUE(mainMenu.IsVisible());
    EXPECT_FALSE(connectionMenu.IsVisible());
    EXPECT_FALSE(settingsMenu.IsVisible());

    // Transition to connection menu
    mainMenu.Hide();
    connectionMenu.Show();
    EXPECT_FALSE(mainMenu.IsVisible());
    EXPECT_TRUE(connectionMenu.IsVisible());
    EXPECT_FALSE(settingsMenu.IsVisible());

    // Back to main menu
    connectionMenu.Hide();
    mainMenu.Show();
    EXPECT_TRUE(mainMenu.IsVisible());
    EXPECT_FALSE(connectionMenu.IsVisible());
    EXPECT_FALSE(settingsMenu.IsVisible());
}

TEST_F(MenuTestFixture, SettingsMenuCallbackChain) {
    Game::SettingsMenu menu(*uiFactory, *graphics);
    menu.Initialize();

    int pingChangeCount = 0;
    int fpsChangeCount = 0;
    int targetFpsChangeCount = 0;

    menu.SetOnShowPingChanged([&](bool) { pingChangeCount++; });
    menu.SetOnShowFpsChanged([&](bool) { fpsChangeCount++; });
    menu.SetOnTargetFpsChanged([&](uint32_t) { targetFpsChangeCount++; });

    menu.SetShowPing(false);
    menu.SetShowPing(true);
    EXPECT_EQ(pingChangeCount, 2);

    menu.SetShowFps(false);
    menu.SetShowFps(true);
    EXPECT_EQ(fpsChangeCount, 2);

    menu.SetTargetFps(120);
    menu.SetTargetFps(60);
    EXPECT_EQ(targetFpsChangeCount, 2);
}
