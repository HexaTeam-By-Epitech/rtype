/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** AuthenticationTest.cpp - Complete authentication system tests
*/

#include <gtest/gtest.h>
#include <chrono>
#include <fstream>
#include <memory>
#include <thread>
#include "server/Sessions/Auth/AuthService.hpp"
#include "server/Sessions/SessionManager/SessionManager.hpp"

using namespace server;

// ============================================================================
// Authentication Tests - Registration
// ============================================================================

class AuthenticationRegistrationTest : public ::testing::Test {
   protected:
    void SetUp() override {
        // Use a test-specific accounts file
        testAccountsFile = "test_accounts_registration.dat";

        // Clean up any existing test file
        std::remove(testAccountsFile.c_str());

        authService = std::make_shared<AuthService>(testAccountsFile);
    }

    void TearDown() override {
        authService.reset();
        // Clean up test file
        std::remove(testAccountsFile.c_str());
    }

    std::shared_ptr<AuthService> authService;
    std::string testAccountsFile;
};

TEST_F(AuthenticationRegistrationTest, RegisterNewUser) {
    // Register a new user
    bool result = authService->registerUser("testuser", "testpass");
    EXPECT_TRUE(result) << "Should successfully register new user";
}

TEST_F(AuthenticationRegistrationTest, RegisterMultipleUsers) {
    EXPECT_TRUE(authService->registerUser("user1", "pass1"));
    EXPECT_TRUE(authService->registerUser("user2", "pass2"));
    EXPECT_TRUE(authService->registerUser("user3", "pass3"));
}

TEST_F(AuthenticationRegistrationTest, RegisterDuplicateUser) {
    // Register user first time
    EXPECT_TRUE(authService->registerUser("duplicate", "password"));

    // Try to register same username again
    EXPECT_FALSE(authService->registerUser("duplicate", "different_pass"))
        << "Should fail when registering duplicate username";
}

TEST_F(AuthenticationRegistrationTest, RegisterWithEmptyUsername) {
    EXPECT_FALSE(authService->registerUser("", "password")) << "Should fail with empty username";
}

TEST_F(AuthenticationRegistrationTest, RegisterWithEmptyPassword) {
    EXPECT_FALSE(authService->registerUser("username", "")) << "Should fail with empty password";
}

TEST_F(AuthenticationRegistrationTest, RegisterWithShortUsername) {
    EXPECT_FALSE(authService->registerUser("ab", "validpass"))
        << "Should fail with username shorter than 3 characters";
}

TEST_F(AuthenticationRegistrationTest, RegisterWithShortPassword) {
    EXPECT_FALSE(authService->registerUser("validuser", "abc"))
        << "Should fail with password shorter than 4 characters";
}

TEST_F(AuthenticationRegistrationTest, RegisterValidMinimumLength) {
    EXPECT_TRUE(authService->registerUser("abc", "abcd"))
        << "Should succeed with minimum valid lengths (3 chars username, 4 chars password)";
}

// ============================================================================
// Authentication Tests - Login
// ============================================================================

class AuthenticationLoginTest : public ::testing::Test {
   protected:
    void SetUp() override {
        testAccountsFile = "test_accounts_login.dat";
        std::remove(testAccountsFile.c_str());

        authService = std::make_shared<AuthService>(testAccountsFile);

        // Pre-register some test accounts
        authService->registerUser("testuser", "testpass");
        authService->registerUser("admin", "admin123");
        authService->registerUser("player1", "secret");
    }

    void TearDown() override {
        authService.reset();
        std::remove(testAccountsFile.c_str());
    }

    std::shared_ptr<AuthService> authService;
    std::string testAccountsFile;
};

TEST_F(AuthenticationLoginTest, LoginWithValidCredentials) {
    EXPECT_TRUE(authService->authenticate("testuser", "testpass"))
        << "Should authenticate with correct credentials";
}

TEST_F(AuthenticationLoginTest, LoginWithWrongPassword) {
    EXPECT_FALSE(authService->authenticate("testuser", "wrongpass")) << "Should fail with incorrect password";
}

TEST_F(AuthenticationLoginTest, LoginWithNonExistentUser) {
    EXPECT_FALSE(authService->authenticate("nonexistent", "anypass"))
        << "Should fail with non-existent username";
}

TEST_F(AuthenticationLoginTest, LoginMultipleAccounts) {
    EXPECT_TRUE(authService->authenticate("testuser", "testpass"));
    EXPECT_TRUE(authService->authenticate("admin", "admin123"));
    EXPECT_TRUE(authService->authenticate("player1", "secret"));
}

TEST_F(AuthenticationLoginTest, LoginWithEmptyUsername) {
    EXPECT_FALSE(authService->authenticate("", "testpass")) << "Should fail with empty username";
}

TEST_F(AuthenticationLoginTest, LoginWithEmptyPassword) {
    EXPECT_FALSE(authService->authenticate("testuser", "")) << "Should fail with empty password";
}

TEST_F(AuthenticationLoginTest, LoginCaseSensitive) {
    // Register lowercase
    authService->registerUser("lowercase", "password");

    // Try to login with different case
    EXPECT_FALSE(authService->authenticate("LowerCase", "password")) << "Username should be case-sensitive";
}

TEST_F(AuthenticationLoginTest, LoginAfterRegister) {
    // Register a new user
    EXPECT_TRUE(authService->registerUser("newuser", "newpass"));

    // Immediately try to login
    EXPECT_TRUE(authService->authenticate("newuser", "newpass"))
        << "Should be able to login immediately after registration";
}

// ============================================================================
// Authentication Tests - Guest Login
// ============================================================================

class AuthenticationGuestTest : public ::testing::Test {
   protected:
    void SetUp() override {
        testAccountsFile = "test_accounts_guest.dat";
        std::remove(testAccountsFile.c_str());

        authService = std::make_shared<AuthService>(testAccountsFile);

        // Guest account should exist by default
        // When no accounts file exists, default accounts are created
    }

    void TearDown() override {
        authService.reset();
        std::remove(testAccountsFile.c_str());
    }

    std::shared_ptr<AuthService> authService;
    std::string testAccountsFile;
};

TEST_F(AuthenticationGuestTest, GuestAccountExists) {
    // Guest login should work without any account in accounts.dat
    EXPECT_TRUE(authService->authenticate("guest", "guest"))
        << "Guest should be able to login without account registration";
}

TEST_F(AuthenticationGuestTest, MultipleGuestLogins) {
    // Multiple guest logins should all succeed
    EXPECT_TRUE(authService->authenticate("guest", "guest"));
    EXPECT_TRUE(authService->authenticate("guest", "guest"));
    EXPECT_TRUE(authService->authenticate("guest", "guest"));
}

TEST_F(AuthenticationGuestTest, GuestWrongPassword) {
    EXPECT_FALSE(authService->authenticate("guest", "wrongpass"))
        << "Guest login should fail with wrong password (must use 'guest' as password)";
}

TEST_F(AuthenticationGuestTest, CannotRegisterGuest) {
    // Should not be able to register "guest" as a regular account
    EXPECT_FALSE(authService->registerUser("guest", "anypassword"))
        << "Should not allow registering 'guest' as it's reserved for anonymous access";
}

// ============================================================================
// Integration Tests - Full Authentication Flow
// ============================================================================

class AuthenticationIntegrationTest : public ::testing::Test {
   protected:
    void SetUp() override {
        testAccountsFile = "test_accounts_integration.dat";
        std::remove(testAccountsFile.c_str());

        authService = std::make_shared<AuthService>(testAccountsFile);
        sessionManager = std::make_shared<SessionManager>(authService);
    }

    void TearDown() override {
        sessionManager.reset();
        authService.reset();
        std::remove(testAccountsFile.c_str());
    }

    std::shared_ptr<AuthService> authService;
    std::shared_ptr<SessionManager> sessionManager;
    std::string testAccountsFile;
};

TEST_F(AuthenticationIntegrationTest, RegisterThenLoginFlow) {
    // Step 1: Register new account
    EXPECT_TRUE(authService->registerUser("newplayer", "password123"));

    // Step 2: Login with registered account
    std::string sessionId = sessionManager->authenticateAndCreateSession("newplayer", "password123");
    EXPECT_FALSE(sessionId.empty()) << "Should create session after successful login";

    // Step 3: Verify session exists
    auto session = sessionManager->getSession(sessionId);
    EXPECT_NE(session, nullptr);
    EXPECT_TRUE(session->isActive());
}

TEST_F(AuthenticationIntegrationTest, LoginFailedNoSession) {
    // Try to login with non-existent account
    std::string sessionId = sessionManager->authenticateAndCreateSession("nonexistent", "wrongpass");
    EXPECT_TRUE(sessionId.empty()) << "Should not create session for failed login";
}

TEST_F(AuthenticationIntegrationTest, GuestLoginWithSession) {
    // Guest login should work with SessionManager
    std::string sessionId = sessionManager->authenticateAndCreateSession("guest", "guest");
    EXPECT_FALSE(sessionId.empty()) << "Guest should be able to login and get session";

    auto session = sessionManager->getSession(sessionId);
    EXPECT_NE(session, nullptr);
}

TEST_F(AuthenticationIntegrationTest, MultipleUsersSessions) {
    // Register multiple users first
    EXPECT_TRUE(authService->registerUser("player1", "pass1"));
    EXPECT_TRUE(authService->registerUser("player2", "pass2"));

    // Login all users (including guest)
    std::string session1 = sessionManager->authenticateAndCreateSession("player1", "pass1");
    std::string session2 = sessionManager->authenticateAndCreateSession("player2", "pass2");
    std::string sessionGuest = sessionManager->authenticateAndCreateSession("guest", "guest");

    // All sessions should be created successfully
    EXPECT_FALSE(session1.empty()) << "Player1 should get a session";
    EXPECT_FALSE(session2.empty()) << "Player2 should get a session";
    EXPECT_FALSE(sessionGuest.empty()) << "Guest should get a session";

    // All sessions should be unique
    EXPECT_NE(session1, session2);
    EXPECT_NE(session1, sessionGuest);
    EXPECT_NE(session2, sessionGuest);

    // All sessions should exist
    EXPECT_NE(sessionManager->getSession(session1), nullptr);
    EXPECT_NE(sessionManager->getSession(session2), nullptr);
    EXPECT_NE(sessionManager->getSession(sessionGuest), nullptr);
}

TEST_F(AuthenticationIntegrationTest, TokenGenerationAfterAuth) {
    // Authenticate user
    authService->registerUser("tokenuser", "tokenpass");
    authService->authenticate("tokenuser", "tokenpass");

    // Generate token
    std::string token = authService->generateToken("tokenuser");
    EXPECT_FALSE(token.empty());

    // Validate token
    EXPECT_TRUE(authService->validateToken(token));

    // Token should contain username
    EXPECT_NE(token.find("tokenuser"), std::string::npos) << "Token should contain username as prefix";
}

TEST_F(AuthenticationIntegrationTest, SessionCleanupAfterLogout) {
    // Login
    std::string sessionId = sessionManager->authenticateAndCreateSession("guest", "guest");
    EXPECT_NE(sessionManager->getSession(sessionId), nullptr);

    // Logout (remove session)
    sessionManager->removeSession(sessionId);
    EXPECT_EQ(sessionManager->getSession(sessionId), nullptr) << "Session should be removed after logout";
}

TEST_F(AuthenticationIntegrationTest, UserAuthenticationTracking) {
    // Register and authenticate
    authService->registerUser("trackuser", "trackpass");
    authService->authenticate("trackuser", "trackpass");

    // User should be marked as authenticated
    EXPECT_TRUE(authService->isUserAuthenticated("trackuser"));

    // Non-authenticated user
    EXPECT_FALSE(authService->isUserAuthenticated("notloggedin"));
}

// ============================================================================
// Security Tests
// ============================================================================

class AuthenticationSecurityTest : public ::testing::Test {
   protected:
    void SetUp() override {
        testAccountsFile = "test_accounts_security.dat";
        std::remove(testAccountsFile.c_str());
        authService = std::make_shared<AuthService>(testAccountsFile);
    }

    void TearDown() override {
        authService.reset();
        std::remove(testAccountsFile.c_str());
    }

    std::shared_ptr<AuthService> authService;
    std::string testAccountsFile;
};

TEST_F(AuthenticationSecurityTest, CannotRegisterGuestAccount) {
    // Try to register with "guest" username (should fail as it's reserved)
    EXPECT_FALSE(authService->registerUser("guest", "newpassword"))
        << "Should not be able to register 'guest' as it's reserved for anonymous access";

    // Guest should still work with original password
    EXPECT_TRUE(authService->authenticate("guest", "guest"));
}

TEST_F(AuthenticationSecurityTest, TokenRevocation) {
    authService->registerUser("secureuser", "securepass");
    authService->authenticate("secureuser", "securepass");

    std::string token = authService->generateToken("secureuser");
    EXPECT_TRUE(authService->validateToken(token));

    // Revoke token
    authService->revokeToken(token);
    EXPECT_FALSE(authService->validateToken(token)) << "Token should be invalid after revocation";
}

TEST_F(AuthenticationSecurityTest, DifferentUsersGetDifferentTokens) {
    authService->registerUser("user1", "pass1");
    authService->registerUser("user2", "pass2");

    authService->authenticate("user1", "pass1");
    authService->authenticate("user2", "pass2");

    std::string token1 = authService->generateToken("user1");
    std::string token2 = authService->generateToken("user2");

    EXPECT_NE(token1, token2) << "Different users should get different tokens";
}
