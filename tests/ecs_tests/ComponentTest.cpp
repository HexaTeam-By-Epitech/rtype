/*
** EPITECH PROJECT, 2025
** RTYPE
** File description:
** ComponentTest
*/

#include <gtest/gtest.h>
#include "../../common/ECS/Components/IComponent.hpp"

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

// Test that each component type has a unique ID
TEST(ComponentTypeTest, EachComponentTypeHasUniqueID) {
    ecs::ComponentType typeA = ecs::getComponentType<TestComponentA>();
    ecs::ComponentType typeB = ecs::getComponentType<TestComponentB>();
    ecs::ComponentType typeC = ecs::getComponentType<TestComponentC>();

    EXPECT_NE(typeA, typeB);
    EXPECT_NE(typeA, typeC);
    EXPECT_NE(typeB, typeC);
}

// Test that the same type always returns the same ID
TEST(ComponentTypeTest, SameComponentTypeReturnsSameID) {
    ecs::ComponentType typeA1 = ecs::getComponentType<TestComponentA>();
    ecs::ComponentType typeA2 = ecs::getComponentType<TestComponentA>();

    EXPECT_EQ(typeA1, typeA2);
}

// Test that getType() returns the correct ID
TEST(ComponentTypeTest, GetTypeReturnsCorrectID) {
    TestComponentA compA;
    TestComponentB compB;

    ecs::ComponentType expectedA = ecs::getComponentType<TestComponentA>();
    ecs::ComponentType expectedB = ecs::getComponentType<TestComponentB>();

    EXPECT_EQ(compA.getType(), expectedA);
    EXPECT_EQ(compB.getType(), expectedB);
}

// Test that different instances of the same type have the same ID
TEST(ComponentTypeTest, DifferentInstancesSameType) {
    TestComponentA comp1;
    TestComponentA comp2;

    EXPECT_EQ(comp1.getType(), comp2.getType());
}

// Test polymorphism with IComponent
TEST(ComponentTypeTest, PolymorphismWorks) {
    ecs::IComponent *compA = new TestComponentA();
    ecs::IComponent *compB = new TestComponentB();

    EXPECT_NE(compA->getType(), compB->getType());
    EXPECT_EQ(compA->getType(), ecs::getComponentType<TestComponentA>());
    EXPECT_EQ(compB->getType(), ecs::getComponentType<TestComponentB>());

    delete compA;
    delete compB;
}
