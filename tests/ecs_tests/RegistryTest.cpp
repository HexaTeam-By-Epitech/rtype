/*
** EPITECH PROJECT, 2025
** rtype
** File description:
** RegistryTest
*/

#include <gtest/gtest.h>

#include <iostream>
#include <set>
#include <vector>
#include "IComponent.hpp"
#include "Registry.hpp"

// Simple test components
class TestComponentA : public ecs::IComponent {
   public:
    ecs::ComponentType getType() const override { return ecs::getComponentType<TestComponentA>(); }
};

class TestComponentB : public ecs::IComponent {
   public:
    ecs::ComponentType getType() const override { return ecs::getComponentType<TestComponentB>(); }
};

class TestComponentC : public ecs::IComponent {
   public:
    ecs::ComponentType getType() const override { return ecs::getComponentType<TestComponentC>(); }
};

TEST(RegistryTest, NewEntityNonZeroAddress) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address nullAddr = 0;

    for (int i = 0; i < 100; i++) {
        ASSERT_NE(reg.newEntity(), nullAddr);
    }
}

TEST(RegistryTest, NewEntityUniqueAddress) {
    ecs::Registry reg = ecs::Registry();
    std::vector<ecs::Address> addrs;

    for (int i = 0; i < 100; i++) {
        addrs.push_back(reg.newEntity());
    }

    std::set<ecs::Address> uniqueAddrs(addrs.begin(), addrs.end());
    ASSERT_EQ(addrs.size(), uniqueAddrs.size());
}

TEST(RegistryTest, GetSignatureNewEntity) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address addr = reg.newEntity();
    ecs::Signature sig = reg.getSignature(addr);

    ASSERT_EQ(sig.count(), 0);
}

TEST(RegistryTest, GetSignatureInvalidAddress) {
    ecs::Registry reg = ecs::Registry();
    ecs::Signature sig = reg.getSignature(12345);

    ASSERT_EQ(sig.count(), 0);
}

TEST(RegistryTest, AddSingleComponent) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address addr = reg.newEntity();

    reg.addEntityProp<TestComponentA>(addr);
    ecs::Signature sig = reg.getSignature(addr);

    ASSERT_EQ(sig.count(), 1);
}

TEST(RegistryTest, AddMultipleComponents) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address addr = reg.newEntity();

    reg.addEntityProp<TestComponentA>(addr);
    reg.addEntityProp<TestComponentB>(addr);
    reg.addEntityProp<TestComponentC>(addr);

    ecs::Signature sig = reg.getSignature(addr);
    std::cout << sig << std::endl;
    ASSERT_EQ(sig.count(), 3);
}

TEST(RegistryTest, AddDuplicateComponent) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address addr = reg.newEntity();

    reg.addEntityProp<TestComponentA>(addr);
    reg.addEntityProp<TestComponentA>(addr);

    ecs::Signature sig = reg.getSignature(addr);
    ASSERT_EQ(sig.count(), 1);
}

TEST(RegistryTest, DestroyEntity) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address addr = reg.newEntity();

    reg.addEntityProp<TestComponentA>(addr);
    ASSERT_EQ(reg.getSignature(addr).count(), 1);

    reg.destroyEntity(addr);
    ASSERT_EQ(reg.getSignature(addr).count(), 0);
}

TEST(RegistryTest, DestroyNonExistentEntity) {
    ecs::Registry reg = ecs::Registry();

    ASSERT_NO_THROW(reg.destroyEntity(99999));
}

TEST(RegistryTest, MultipleEntitiesWithSameComponents) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address addr1 = reg.newEntity();
    ecs::Address addr2 = reg.newEntity();

    reg.addEntityProp<TestComponentA>(addr1);
    reg.addEntityProp<TestComponentA>(addr2);

    ecs::Signature sig1 = reg.getSignature(addr1);
    ecs::Signature sig2 = reg.getSignature(addr2);

    ASSERT_EQ(sig1, sig2);
}

TEST(RegistryTest, MultipleEntitiesWithDifferentComponents) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address addr1 = reg.newEntity();
    ecs::Address addr2 = reg.newEntity();

    reg.addEntityProp<TestComponentA>(addr1);
    reg.addEntityProp<TestComponentB>(addr2);

    ecs::Signature sig1 = reg.getSignature(addr1);
    ecs::Signature sig2 = reg.getSignature(addr2);

    ASSERT_NE(sig1, sig2);
}

TEST(RegistryTest, AddComponentToInvalidEntity) {
    ecs::Registry reg = ecs::Registry();

    ASSERT_NO_THROW(reg.addEntityProp<TestComponentA>(99999));
}
