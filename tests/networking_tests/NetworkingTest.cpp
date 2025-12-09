/*
** EPITECH PROJECT, 2025
** Created by GitHub Copilot on 08/12/2025.
** File description:
** NetworkingTest.cpp
*/

#include <gtest/gtest.h>
#include "NetworkFactory.hpp"

// ============================================================================
// NetworkFactory Tests
// ============================================================================

TEST(NetworkFactoryTest, InitializeNetworking) {
    EXPECT_TRUE(initializeNetworking());
    // Should be idempotent
    EXPECT_TRUE(initializeNetworking());
}

TEST(NetworkFactoryTest, DeinitializeNetworking) {
    initializeNetworking();
    EXPECT_NO_THROW(deinitializeNetworking());
    // Should be idempotent
    EXPECT_NO_THROW(deinitializeNetworking());
}

TEST(NetworkFactoryTest, CreateAddress) {
    initializeNetworking();
    auto addr = createAddress("127.0.0.1", 4242);
    ASSERT_NE(addr, nullptr);
    EXPECT_EQ(addr->getPort(), 4242);
    deinitializeNetworking();
}

TEST(NetworkFactoryTest, CreateAddressInvalidHost) {
    initializeNetworking();
    // Invalid hostname should throw an exception
    EXPECT_THROW(createAddress("invalid.invalid", 4242), std::runtime_error);
    deinitializeNetworking();
}

TEST(NetworkFactoryTest, CreatePacketFromData) {
    initializeNetworking();
    std::vector<uint8_t> data = {1, 2, 3, 4, 5};
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    ASSERT_NE(packet, nullptr);
    EXPECT_EQ(packet->getSize(), 5);
    deinitializeNetworking();
}

TEST(NetworkFactoryTest, CreatePacketEmpty) {
    initializeNetworking();
    std::vector<uint8_t> data;
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    ASSERT_NE(packet, nullptr);
    EXPECT_EQ(packet->getSize(), 0);
    deinitializeNetworking();
}

TEST(NetworkFactoryTest, CreateServerHost) {
    initializeNetworking();
    auto addr = createAddress("127.0.0.1", 4242);
    auto host = createServerHost(*addr, 10);
    EXPECT_NE(host, nullptr);
    deinitializeNetworking();
}

TEST(NetworkFactoryTest, CreateClientHost) {
    initializeNetworking();
    auto host = createClientHost();
    EXPECT_NE(host, nullptr);
    deinitializeNetworking();
}

// ============================================================================
// Address Tests
// ============================================================================

TEST(AddressTest, CreateAddressWithHostAndPort) {
    initializeNetworking();
    auto addr = createAddress("127.0.0.1", 4242);
    ASSERT_NE(addr, nullptr);
    EXPECT_EQ(addr->getPort(), 4242);
    deinitializeNetworking();
}

TEST(AddressTest, CreateAddressWithDifferentPort) {
    initializeNetworking();
    auto addr = createAddress("localhost", 8080);
    ASSERT_NE(addr, nullptr);
    EXPECT_EQ(addr->getPort(), 8080);
    deinitializeNetworking();
}

TEST(AddressTest, GetPort) {
    initializeNetworking();
    auto addr = createAddress("0.0.0.0", 9999);
    ASSERT_NE(addr, nullptr);
    EXPECT_EQ(addr->getPort(), 9999);
    deinitializeNetworking();
}

// ============================================================================
// Packet Tests
// ============================================================================

TEST(PacketTest, CreatePacketWithData) {
    initializeNetworking();
    std::vector<uint8_t> data = {10, 20, 30};
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    ASSERT_NE(packet, nullptr);
    EXPECT_EQ(packet->getSize(), 3);
    EXPECT_FALSE(packet->getData().empty());
    deinitializeNetworking();
}

TEST(PacketTest, CreatePacketWithEmptyData) {
    initializeNetworking();
    std::vector<uint8_t> data;
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    ASSERT_NE(packet, nullptr);
    EXPECT_EQ(packet->getSize(), 0);
    deinitializeNetworking();
}

TEST(PacketTest, GetSize) {
    initializeNetworking();
    std::vector<uint8_t> data = {1, 2, 3, 4, 5, 6};
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    EXPECT_EQ(packet->getSize(), 6);
    deinitializeNetworking();
}

TEST(PacketTest, GetData) {
    initializeNetworking();
    std::vector<uint8_t> data = {0xAA, 0xBB, 0xCC};
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    const auto &packetData = packet->getData();
    ASSERT_FALSE(packetData.empty());
    EXPECT_EQ(packetData[0], 0xAA);
    EXPECT_EQ(packetData[1], 0xBB);
    EXPECT_EQ(packetData[2], 0xCC);
    deinitializeNetworking();
}

// ============================================================================
// ENetPeer Tests
// ============================================================================

TEST(ENetPeerTest, SendPacket) {
    initializeNetworking();

    // Create server
    auto serverAddr = createAddress("127.0.0.1", 4243);
    auto serverHost = createServerHost(*serverAddr, 1);
    ASSERT_NE(serverHost, nullptr);

    // Create client
    auto clientHost = createClientHost();
    ASSERT_NE(clientHost, nullptr);

    // Connect
    auto clientAddr = createAddress("127.0.0.1", 4243);
    IPeer *peer = clientHost->connect(*clientAddr, 1, 0);
    ASSERT_NE(peer, nullptr);

    // Let connection establish
    for (int i = 0; i < 10; ++i) {
        clientHost->service(100);
        serverHost->service(100);
    }

    // Send packet
    std::vector<uint8_t> data = {0xDE, 0xAD, 0xBE, 0xEF};
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    EXPECT_NO_THROW(peer->send(std::move(packet), 0));

    deinitializeNetworking();
}

TEST(ENetPeerTest, Disconnect) {
    initializeNetworking();

    auto serverAddr = createAddress("127.0.0.1", 4244);
    auto serverHost = createServerHost(*serverAddr, 1);
    auto clientHost = createClientHost();

    auto clientAddr = createAddress("127.0.0.1", 4244);
    IPeer *peer = clientHost->connect(*clientAddr, 1, 0);

    // Let connection establish
    for (int i = 0; i < 10; ++i) {
        clientHost->service(100);
        serverHost->service(100);
    }

    EXPECT_NO_THROW(peer->disconnect(0));

    deinitializeNetworking();
}

TEST(ENetPeerTest, DisconnectNow) {
    initializeNetworking();

    auto serverAddr = createAddress("127.0.0.1", 4245);
    auto serverHost = createServerHost(*serverAddr, 1);
    auto clientHost = createClientHost();

    auto clientAddr = createAddress("127.0.0.1", 4245);
    IPeer *peer = clientHost->connect(*clientAddr, 1, 0);

    for (int i = 0; i < 10; ++i) {
        clientHost->service(100);
        serverHost->service(100);
    }

    EXPECT_NO_THROW(peer->disconnectNow(0));

    deinitializeNetworking();
}

// ============================================================================
// IHost Tests (via createServerHost and createClientHost)
// ============================================================================

TEST(HostTest, ServerHostCreation) {
    initializeNetworking();
    auto addr = createAddress("127.0.0.1", 4247);
    auto host = createServerHost(*addr, 10);
    EXPECT_NE(host, nullptr);
    EXPECT_EQ(host->getPeerCount(), 0);
    deinitializeNetworking();
}

TEST(HostTest, ClientHostCreation) {
    initializeNetworking();
    auto host = createClientHost();
    EXPECT_NE(host, nullptr);
    EXPECT_EQ(host->getPeerCount(), 0);
    deinitializeNetworking();
}

TEST(HostTest, Connect) {
    initializeNetworking();

    auto serverAddr = createAddress("127.0.0.1", 4248);
    auto serverHost = createServerHost(*serverAddr, 1);
    auto clientHost = createClientHost();

    auto connectAddr = createAddress("127.0.0.1", 4248);
    IPeer *peer = clientHost->connect(*connectAddr, 1, 0);
    EXPECT_NE(peer, nullptr);

    deinitializeNetworking();
}

TEST(HostTest, Service) {
    initializeNetworking();

    auto addr = createAddress("127.0.0.1", 4249);
    auto host = createServerHost(*addr, 1);

    auto event = host->service(0);
    EXPECT_FALSE(event.has_value());

    deinitializeNetworking();
}

TEST(HostTest, ServiceWithTimeout) {
    initializeNetworking();

    auto addr = createAddress("127.0.0.1", 4250);
    auto host = createServerHost(*addr, 1);

    auto event = host->service(100);
    EXPECT_FALSE(event.has_value());

    deinitializeNetworking();
}

TEST(HostTest, Broadcast) {
    initializeNetworking();

    auto addr = createAddress("127.0.0.1", 4251);
    auto host = createServerHost(*addr, 2);

    std::vector<uint8_t> data = {1, 2, 3};
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));

    EXPECT_NO_THROW(host->broadcast(std::move(packet), 0));

    deinitializeNetworking();
}

TEST(HostTest, Flush) {
    initializeNetworking();

    auto addr = createAddress("127.0.0.1", 4252);
    auto host = createServerHost(*addr, 1);

    EXPECT_NO_THROW(host->flush());

    deinitializeNetworking();
}

TEST(HostTest, GetAddress) {
    initializeNetworking();

    auto addr = createAddress("127.0.0.1", 4253);
    auto host = createServerHost(*addr, 1);

    const IAddress &hostAddr = host->getAddress();
    EXPECT_EQ(hostAddr.getPort(), 4253);

    deinitializeNetworking();
}

TEST(HostTest, ConnectionEstablishment) {
    initializeNetworking();

    auto serverAddr = createAddress("127.0.0.1", 4254);
    auto serverHost = createServerHost(*serverAddr, 1);
    auto clientHost = createClientHost();

    auto connectAddr = createAddress("127.0.0.1", 4254);
    IPeer *clientPeer = clientHost->connect(*connectAddr, 1, 0);
    ASSERT_NE(clientPeer, nullptr);

    // Process events to establish connection
    bool connected = false;
    for (int i = 0; i < 50 && !connected; ++i) {
        auto clientEvent = clientHost->service(10);
        auto serverEvent = serverHost->service(10);

        if (serverEvent.has_value() && serverEvent->type == NetworkEventType::CONNECT) {
            connected = true;
            EXPECT_NE(serverEvent->peer, nullptr);
        }
    }

    EXPECT_TRUE(connected);

    deinitializeNetworking();
}

TEST(HostTest, PacketTransmission) {
    initializeNetworking();

    auto serverAddr = createAddress("127.0.0.1", 4255);
    auto serverHost = createServerHost(*serverAddr, 1);
    auto clientHost = createClientHost();

    auto connectAddr = createAddress("127.0.0.1", 4255);
    IPeer *clientPeer = clientHost->connect(*connectAddr, 1, 0);

    // Wait for connection
    IPeer *serverPeer = nullptr;
    for (int i = 0; i < 50; ++i) {
        clientHost->service(10);
        auto serverEvent = serverHost->service(10);
        if (serverEvent.has_value() && serverEvent->type == NetworkEventType::CONNECT) {
            serverPeer = serverEvent->peer;
            break;
        }
    }

    ASSERT_NE(serverPeer, nullptr);

    // Send packet from client to server
    std::vector<uint8_t> data = {0xCA, 0xFE, 0xBA, 0xBE};
    auto packet = createPacket(data, static_cast<uint32_t>(PacketFlag::RELIABLE));
    clientPeer->send(std::move(packet), 0);

    // Wait for packet
    bool received = false;
    for (int i = 0; i < 50 && !received; ++i) {
        clientHost->service(10);
        auto serverEvent = serverHost->service(10);
        if (serverEvent.has_value() && serverEvent->type == NetworkEventType::RECEIVE) {
            received = true;
            ASSERT_NE(serverEvent->packet, nullptr);
            EXPECT_EQ(serverEvent->packet->getSize(), 4);
            const auto &receivedData = serverEvent->packet->getData();
            EXPECT_EQ(receivedData[0], 0xCA);
            EXPECT_EQ(receivedData[1], 0xFE);
            EXPECT_EQ(receivedData[2], 0xBA);
            EXPECT_EQ(receivedData[3], 0xBE);
        }
    }

    EXPECT_TRUE(received);

    deinitializeNetworking();
}
