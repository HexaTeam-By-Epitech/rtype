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

TEST(RegisteryTest, AddTooMuchComponents) {
    ecs::Registry reg = ecs::Registry();
    ecs::Address addr = reg.newEntity();

// Generate 33 distinct test component types (0..32). Each type must be
// distinct so the Registry will attempt to register a new component slot
// for each one.
#define DECL_TEST_COMP(N)                                     \
    struct TestComponent##N : public ecs::IComponent {        \
        ecs::ComponentType getType() const override {         \
            return ecs::getComponentType<TestComponent##N>(); \
        }                                                     \
    };

    DECL_TEST_COMP(0)
    DECL_TEST_COMP(1)
    DECL_TEST_COMP(2)
    DECL_TEST_COMP(3)
    DECL_TEST_COMP(4)
    DECL_TEST_COMP(5)
    DECL_TEST_COMP(6)
    DECL_TEST_COMP(7)
    DECL_TEST_COMP(8)
    DECL_TEST_COMP(9)
    DECL_TEST_COMP(10)
    DECL_TEST_COMP(11)
    DECL_TEST_COMP(12)
    DECL_TEST_COMP(13)
    DECL_TEST_COMP(14)
    DECL_TEST_COMP(15)
    DECL_TEST_COMP(16)
    DECL_TEST_COMP(17)
    DECL_TEST_COMP(18)
    DECL_TEST_COMP(19)
    DECL_TEST_COMP(20)
    DECL_TEST_COMP(21)
    DECL_TEST_COMP(22)
    DECL_TEST_COMP(23)
    DECL_TEST_COMP(24)
    DECL_TEST_COMP(25)
    DECL_TEST_COMP(26)
    DECL_TEST_COMP(27)
    DECL_TEST_COMP(28)
    DECL_TEST_COMP(29)
    DECL_TEST_COMP(30)
    DECL_TEST_COMP(31)
    DECL_TEST_COMP(32)

#undef DECL_TEST_COMP

    // Register 32 distinct component types (TestComponent0 through TestComponent31),
    // which should succeed as it exactly matches N_MAX_COMPONENTS (32) limit.
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent0>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent1>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent2>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent3>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent4>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent5>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent6>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent7>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent8>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent9>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent10>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent11>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent12>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent13>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent14>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent15>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent16>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent17>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent18>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent19>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent20>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent21>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent22>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent23>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent24>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent25>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent26>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent27>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent28>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent29>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent30>(addr));
    ASSERT_NO_THROW(reg.addEntityProp<TestComponent31>(addr));

    // The 33rd distinct component registration (index 32) should fail because
    // it exceeds N_MAX_COMPONENTS and Registry::_registerComponent returns
    // a zero signature, which causes addEntityProp to throw a runtime_error.
    ASSERT_THROW(reg.addEntityProp<TestComponent32>(addr), std::runtime_error);
}
