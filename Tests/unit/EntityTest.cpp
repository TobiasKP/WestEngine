#include <Entity/Entity.h>
#include <Components/ComponentRegistry.hpp>
#include <gtest/gtest.h>

struct TestComponent
{
  int value = 42;
};

// --- Entity (thin handle) tests ---

TEST(Entity, ConstructorSetsId)
{
  Entity entity(7);
  EXPECT_EQ(entity.getId(), 7);
}

TEST(Entity, DefaultsNotDestroyed)
{
  Entity entity(1);
  EXPECT_FALSE(entity.isDestroyed());
}

TEST(Entity, DestroyFlagWorks)
{
  Entity entity(1);
  entity.destroy();
  EXPECT_TRUE(entity.isDestroyed());
}

TEST(Entity, SetAndGetName)
{
  Entity entity(1);
  entity.setName("TestUnit");
  EXPECT_EQ(entity.getName(), "TestUnit");
}

TEST(Entity, DebugEntityFlag)
{
  Entity entity(1);
  EXPECT_FALSE(entity.isDebugEntity());
  entity.debugEntity();
  EXPECT_TRUE(entity.isDebugEntity());
}

// --- ComponentRegistry tests ---

TEST(ComponentRegistry, AddAndGetComponentRoundTrip)
{
  ComponentRegistry registry;
  registry.registerComponent<TestComponent>();

  TestComponent comp;
  comp.value = 99;
  registry.addComponent<TestComponent>(1, std::move(comp));

  TestComponent* result = registry.getComponent<TestComponent>(1);
  ASSERT_NE(result, nullptr);
  EXPECT_EQ(result->value, 99);
}

TEST(ComponentRegistry, GetComponentReturnsNullForMissing)
{
  ComponentRegistry registry;
  registry.registerComponent<TestComponent>();

  TestComponent* result = registry.getComponent<TestComponent>(42);
  EXPECT_EQ(result, nullptr);
}

TEST(ComponentRegistry, RemoveComponent)
{
  ComponentRegistry registry;
  registry.registerComponent<TestComponent>();

  TestComponent comp;
  comp.value = 55;
  registry.addComponent<TestComponent>(1, std::move(comp));

  bool removed = registry.removeComponent<TestComponent>(1);
  EXPECT_TRUE(removed);
  EXPECT_EQ(registry.getComponent<TestComponent>(1), nullptr);
}

TEST(ComponentRegistry, RemoveMissingReturnsFalse)
{
  ComponentRegistry registry;
  registry.registerComponent<TestComponent>();

  EXPECT_FALSE(registry.removeComponent<TestComponent>(999));
}
