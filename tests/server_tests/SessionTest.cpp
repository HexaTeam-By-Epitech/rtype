/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** SessionTest.cpp - Unit tests for Session, SessionManager, and AuthService
*/

#include <gtest/gtest.h>
#include <chrono>
#include <memory>
#include <thread>
#include "server/Sessions/Auth/AuthService.hpp"
#include "server/Sessions/Session/Session.hpp"
#include "server/Sessions/SessionManager/SessionManager.hpp"

using namespace server;

// ============================================================================
// Session Tests
// ============================================================================

class SessionTest : public ::testing::Test {
   protected:
    void SetUp() override { session = std::make_shared<Session>("test-session-1"); }

    void TearDown() override { session.reset(); }

    std::shared_ptr<Session> session;
};

TEST_F(SessionTest, Construction) {
    EXPECT_EQ(session->getId(), "test-session-1");
    EXPECT_EQ(session->getPlayerId(), 0);  // No player associated yet
}

TEST_F(SessionTest, SetAndGetPlayerId) {
    session->setPlayerId(42);
    EXPECT_EQ(session->getPlayerId(), 42);

    session->setPlayerId(100);
    EXPECT_EQ(session->getPlayerId(), 100);
}

TEST_F(SessionTest, SetAndGetPlayerName) {
    session->setPlayerId(42);
    EXPECT_EQ(session->getPlayerId(), 42);

    session->setPlayerId(100);
    EXPECT_EQ(session->getPlayerId(), 100);
}

TEST_F(SessionTest, IsActive) {
    EXPECT_TRUE(session->isActive());  // Active by default

    session->setActive(false);
    EXPECT_FALSE(session->isActive());

    session->setActive(true);
    EXPECT_TRUE(session->isActive());
}

TEST_F(SessionTest, CompleteSession) {
    session->setPlayerId(123);

    EXPECT_EQ(session->getId(), "test-session-1");
    EXPECT_EQ(session->getPlayerId(), 123);
    EXPECT_TRUE(session->isActive());
}

// ============================================================================
// SessionManager Tests
// ============================================================================

class SessionManagerTest : public ::testing::Test {
   protected:
    void SetUp() override { manager = std::make_shared<SessionManager>(); }

    void TearDown() override { manager.reset(); }

    std::shared_ptr<SessionManager> manager;
};

TEST_F(SessionManagerTest, CreateSession) {
    auto session = manager->createSession("session-1");
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(session->getId(), "session-1");
}

TEST_F(SessionManagerTest, CreateMultipleSessions) {
    auto session1 = manager->createSession("session-1");
    auto session2 = manager->createSession("session-2");
    auto session3 = manager->createSession("session-3");

    EXPECT_NE(session1, nullptr);
    EXPECT_NE(session2, nullptr);
    EXPECT_NE(session3, nullptr);
    EXPECT_NE(session1, session2);
    EXPECT_NE(session2, session3);
}

TEST_F(SessionManagerTest, CreateDuplicateSession) {
    auto session1 = manager->createSession("duplicate");
    EXPECT_NE(session1, nullptr);

    auto session2 = manager->createSession("duplicate");
    EXPECT_NE(session2, nullptr);
    // Note: Current implementation allows duplicates, might want to change this
}

TEST_F(SessionManagerTest, GetSession) {
    manager->createSession("session-1");

    auto session = manager->getSession("session-1");
    EXPECT_NE(session, nullptr);
    EXPECT_EQ(session->getId(), "session-1");
}

TEST_F(SessionManagerTest, GetNonExistentSession) {
    auto session = manager->getSession("non-existent");
    EXPECT_EQ(session, nullptr);
}

TEST_F(SessionManagerTest, RemoveSession) {
    manager->createSession("session-to-remove");
    EXPECT_NE(manager->getSession("session-to-remove"), nullptr);

    manager->removeSession("session-to-remove");
    EXPECT_EQ(manager->getSession("session-to-remove"), nullptr);
}

TEST_F(SessionManagerTest, RemoveNonExistentSession) {
    EXPECT_NO_THROW(manager->removeSession("non-existent"));
}

TEST_F(SessionManagerTest, SessionLifecycle) {
    // Create
    auto session = manager->createSession("lifecycle-test");
    EXPECT_NE(session, nullptr);

    // Modify
    session->setPlayerId(456);

    // Retrieve and verify
    auto retrieved = manager->getSession("lifecycle-test");
    EXPECT_EQ(retrieved->getPlayerId(), 456);

    // Remove
    manager->removeSession("lifecycle-test");
    EXPECT_EQ(manager->getSession("lifecycle-test"), nullptr);
}

// ============================================================================
// AuthService Tests
// ============================================================================

class AuthServiceTest : public ::testing::Test {
   protected:
    void SetUp() override { authService = std::make_shared<AuthService>(); }

    void TearDown() override { authService.reset(); }

    std::shared_ptr<AuthService> authService;
};

TEST_F(AuthServiceTest, Construction) {
    EXPECT_NE(authService, nullptr);
}

TEST_F(AuthServiceTest, AuthenticateDefaultBehavior) {
    // The default implementation always returns true for any credentials
    EXPECT_TRUE(authService->authenticate("alice", "password123"));
    EXPECT_TRUE(authService->authenticate("bob", "secret456"));
}

TEST_F(AuthServiceTest, GenerateToken) {
    std::string token = authService->generateToken("testuser");
    EXPECT_FALSE(token.empty());
}

TEST_F(AuthServiceTest, GenerateMultipleTokens) {
    std::string token1 = authService->generateToken("user1");
    std::string token2 = authService->generateToken("user2");

    EXPECT_FALSE(token1.empty());
    EXPECT_FALSE(token2.empty());
    EXPECT_NE(token1, token2);
}

TEST_F(AuthServiceTest, ValidateToken) {
    std::string token = authService->generateToken("testuser");
    EXPECT_TRUE(authService->validateToken(token));
}

TEST_F(AuthServiceTest, ValidateInvalidToken) {
    EXPECT_FALSE(authService->validateToken("invalid-token-12345"));
}

TEST_F(AuthServiceTest, RevokeToken) {
    std::string token = authService->generateToken("testuser");
    EXPECT_TRUE(authService->validateToken(token));

    authService->revokeToken(token);
    EXPECT_FALSE(authService->validateToken(token));
}

TEST_F(AuthServiceTest, IsUserAuthenticated) {
    authService->authenticate("testuser", "testpass");
    EXPECT_TRUE(authService->isUserAuthenticated("testuser"));
}

TEST_F(AuthServiceTest, UserNotAuthenticated) {
    EXPECT_FALSE(authService->isUserAuthenticated("nonexistent"));
}

// ============================================================================
// SessionManager with AuthService Tests
// ============================================================================

class SessionManagerAuthTest : public ::testing::Test {
   protected:
    void SetUp() override {
        authService = std::make_shared<AuthService>();
        manager = std::make_shared<SessionManager>(authService);
    }

    void TearDown() override {
        manager.reset();
        authService.reset();
    }

    std::shared_ptr<AuthService> authService;
    std::shared_ptr<SessionManager> manager;
};

TEST_F(SessionManagerAuthTest, AuthenticateAndCreateSession) {
    std::string sessionId = manager->authenticateAndCreateSession("testuser", "testpass");
    EXPECT_FALSE(sessionId.empty());

    auto session = manager->getSession(sessionId);
    EXPECT_NE(session, nullptr);
}

TEST_F(SessionManagerAuthTest, MultipleAuthentications) {
    std::string session1 = manager->authenticateAndCreateSession("user1", "pass1");
    std::string session2 = manager->authenticateAndCreateSession("user2", "pass2");

    EXPECT_FALSE(session1.empty());
    EXPECT_FALSE(session2.empty());
    EXPECT_NE(session1, session2);
}

TEST_F(SessionManagerAuthTest, GetAuthService) {
    auto retrievedAuth = manager->getAuthService();
    EXPECT_EQ(retrievedAuth, authService);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(SessionIntegrationTest, CompleteAuthenticationFlow) {
    auto authService = std::make_shared<AuthService>();
    auto sessionManager = std::make_shared<SessionManager>(authService);

    // Authenticate and create sessions
    std::string session1 = sessionManager->authenticateAndCreateSession("player1", "secret1");
    std::string session2 = sessionManager->authenticateAndCreateSession("player2", "secret2");

    EXPECT_FALSE(session1.empty());
    EXPECT_FALSE(session2.empty());

    // Retrieve and verify sessions
    auto s1 = sessionManager->getSession(session1);
    auto s2 = sessionManager->getSession(session2);

    EXPECT_NE(s1, nullptr);
    EXPECT_NE(s2, nullptr);
    EXPECT_TRUE(s1->isActive());
    EXPECT_TRUE(s2->isActive());

    // Clean up
    sessionManager->removeSession(session1);
    sessionManager->removeSession(session2);

    EXPECT_EQ(sessionManager->getSession(session1), nullptr);
    EXPECT_EQ(sessionManager->getSession(session2), nullptr);
}
