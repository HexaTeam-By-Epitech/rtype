/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 08/12/2025.
** File description:
** SerializationTest.cpp
*/

#include <gtest/gtest.h>
#include <cstring>
#include <vector>
#include "Capnp/NetworkMessages.hpp"

// ============================================================================
// NetworkMessages Tests
// ============================================================================

TEST(NetworkMessagesTest, CreateConnectRequest) {
    std::string playerName = "TestPlayer";
    auto data = NetworkMessages::createConnectRequest(playerName);

    EXPECT_FALSE(data.empty());
    EXPECT_GT(data.size(), 0);
}

TEST(NetworkMessagesTest, ParseConnectRequest) {
    std::string playerName = "Alice";
    auto data = NetworkMessages::createConnectRequest(playerName);

    std::string parsedName = NetworkMessages::parseConnectRequest(data);
    EXPECT_EQ(parsedName, playerName);
}

TEST(NetworkMessagesTest, CreateConnectResponse) {
    std::string message = "Welcome to R-Type!";
    auto data = NetworkMessages::createConnectResponse(message);

    EXPECT_FALSE(data.empty());
    EXPECT_GT(data.size(), 0);
}

TEST(NetworkMessagesTest, ParseConnectResponse) {
    std::string message = "Connection successful";
    auto data = NetworkMessages::createConnectResponse(message);

    std::string parsedMessage = NetworkMessages::parseConnectResponse(data);
    EXPECT_EQ(parsedMessage, message);
}

TEST(NetworkMessagesTest, GetMessageTypeConnectRequest) {
    auto data = NetworkMessages::createConnectRequest("Player1");
    auto type = NetworkMessages::getMessageType(data);

    EXPECT_EQ(type, NetworkMessages::MessageType::HANDSHAKE_REQUEST);
}

TEST(NetworkMessagesTest, GetMessageTypeConnectResponse) {
    auto data = NetworkMessages::createConnectResponse("OK");
    auto type = NetworkMessages::getMessageType(data);

    EXPECT_EQ(type, NetworkMessages::MessageType::HANDSHAKE_RESPONSE);
}

TEST(NetworkMessagesTest, EmptyPlayerName) {
    std::string playerName = "";
    auto data = NetworkMessages::createConnectRequest(playerName);

    std::string parsedName = NetworkMessages::parseConnectRequest(data);
    EXPECT_EQ(parsedName, playerName);
}

TEST(NetworkMessagesTest, LongPlayerName) {
    std::string playerName(1000, 'A');
    auto data = NetworkMessages::createConnectRequest(playerName);

    std::string parsedName = NetworkMessages::parseConnectRequest(data);
    EXPECT_EQ(parsedName, playerName);
}

TEST(NetworkMessagesTest, SpecialCharactersInName) {
    std::string playerName = "Player_123!@#$%";
    auto data = NetworkMessages::createConnectRequest(playerName);

    std::string parsedName = NetworkMessages::parseConnectRequest(data);
    EXPECT_EQ(parsedName, playerName);
}

TEST(NetworkMessagesTest, UnicodeInName) {
    std::string playerName = "玩家123";
    auto data = NetworkMessages::createConnectRequest(playerName);

    std::string parsedName = NetworkMessages::parseConnectRequest(data);
    EXPECT_EQ(parsedName, playerName);
}

TEST(NetworkMessagesTest, MultipleMessagesSerializeDeserialize) {
    std::vector<std::string> names = {"Alice", "Bob", "Charlie", "Diana"};

    for (const auto &name : names) {
        auto data = NetworkMessages::createConnectRequest(name);
        std::string parsed = NetworkMessages::parseConnectRequest(data);
        EXPECT_EQ(parsed, name);
    }
}

TEST(NetworkMessagesTest, LargeMessage) {
    std::string message(10000, 'X');
    auto data = NetworkMessages::createConnectResponse(message);

    std::string parsed = NetworkMessages::parseConnectResponse(data);
    EXPECT_EQ(parsed, message);
}

// ============================================================================
// Integration Tests
// ============================================================================

TEST(SerializationIntegrationTest, CompleteWorkflow) {
    // Simulate client sending connect request
    std::string clientName = "IntegrationTestClient";
    auto requestData = NetworkMessages::createConnectRequest(clientName);

    // Simulate server receiving and parsing
    auto messageType = NetworkMessages::getMessageType(requestData);
    EXPECT_EQ(messageType, NetworkMessages::MessageType::HANDSHAKE_REQUEST);

    std::string receivedName = NetworkMessages::parseConnectRequest(requestData);
    EXPECT_EQ(receivedName, clientName);

    // Simulate server sending response
    std::string responseMsg = "Hello " + receivedName + ", welcome!";
    auto responseData = NetworkMessages::createConnectResponse(responseMsg);

    // Simulate client receiving and parsing
    auto responseType = NetworkMessages::getMessageType(responseData);
    EXPECT_EQ(responseType, NetworkMessages::MessageType::HANDSHAKE_RESPONSE);

    std::string receivedMsg = NetworkMessages::parseConnectResponse(responseData);
    EXPECT_EQ(receivedMsg, responseMsg);
}

TEST(SerializationIntegrationTest, MultipleClients) {
    std::vector<std::string> clientNames = {"Client1", "Client2", "Client3", "Client4", "Client5"};

    for (const auto &name : clientNames) {
        auto requestData = NetworkMessages::createConnectRequest(name);
        std::string parsed = NetworkMessages::parseConnectRequest(requestData);
        EXPECT_EQ(parsed, name);

        std::string response = "Welcome " + name;
        auto responseData = NetworkMessages::createConnectResponse(response);
        std::string parsedResponse = NetworkMessages::parseConnectResponse(responseData);
        EXPECT_EQ(parsedResponse, response);
    }
}

TEST(SerializationIntegrationTest, StressTest) {
    // Test with many rapid serializations
    for (int i = 0; i < 1000; ++i) {
        std::string name = "Player" + std::to_string(i);
        auto data = NetworkMessages::createConnectRequest(name);
        std::string parsed = NetworkMessages::parseConnectRequest(data);
        EXPECT_EQ(parsed, name);
    }
}