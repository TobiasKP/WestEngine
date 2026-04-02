#include <Components/ComponentRegistry.hpp>
#include <gtest/gtest.h>

struct TestComp
{
  int value = 0;
};

class ComponentArrayTest : public ::testing::Test
{
protected:
  ComponentRegistry registry;

  void SetUp() override
  {
    registry.registerComponent<TestComp>();
  }

  void addComp(std::uint32_t id, int val)
  {
    TestComp c;
    c.value = val;
    registry.addComponent<TestComp>(id, std::move(c));
  }

  auto getArray()
  {
    return registry.getComponentArray<TestComp>();
  }
};

// ─── Swap-and-pop: remove middle ─────────────────────────────

TEST_F(ComponentArrayTest, RemoveMiddlePreservesOthers)
{
  addComp(10, 100);
  addComp(20, 200);
  addComp(30, 300);

  registry.removeComponent<TestComp>(20);

  EXPECT_NE(registry.getComponent<TestComp>(10), nullptr);
  EXPECT_EQ(registry.getComponent<TestComp>(10)->value, 100);

  EXPECT_NE(registry.getComponent<TestComp>(30), nullptr);
  EXPECT_EQ(registry.getComponent<TestComp>(30)->value, 300);

  EXPECT_EQ(registry.getComponent<TestComp>(20), nullptr);
}

// ─── Swap-and-pop: remove only element ───────────────────────

TEST_F(ComponentArrayTest, RemoveOnlyElement)
{
  addComp(1, 42);
  EXPECT_TRUE(registry.removeComponent<TestComp>(1));
  EXPECT_EQ(registry.getComponent<TestComp>(1), nullptr);
  EXPECT_EQ(getArray()->getSize(), 0u);
}

// ─── Swap-and-pop: remove last element ───────────────────────

TEST_F(ComponentArrayTest, RemoveLastElementIsNoOpSwap)
{
  addComp(1, 10);
  addComp(2, 20);
  addComp(3, 30);

  // Remove the last-added element (idx 2, which IS the tail)
  EXPECT_TRUE(registry.removeComponent<TestComp>(3));
  EXPECT_EQ(getArray()->getSize(), 2u);

  // Others unchanged
  EXPECT_EQ(registry.getComponent<TestComp>(1)->value, 10);
  EXPECT_EQ(registry.getComponent<TestComp>(2)->value, 20);
  EXPECT_EQ(registry.getComponent<TestComp>(3), nullptr);
}

// ─── getSize reflects operations ─────────────────────────────

TEST_F(ComponentArrayTest, SizeTracksAddAndRemove)
{
  EXPECT_EQ(getArray()->getSize(), 0u);

  addComp(1, 10);
  addComp(2, 20);
  addComp(3, 30);
  EXPECT_EQ(getArray()->getSize(), 3u);

  registry.removeComponent<TestComp>(2);
  EXPECT_EQ(getArray()->getSize(), 2u);
}

// ─── getEntityIdByIdx round-trip ─────────────────────────────

TEST_F(ComponentArrayTest, EntityIdByIdxRoundTrip)
{
  addComp(10, 100);
  addComp(20, 200);

  auto arr = getArray();

  // Walk dense array by index, verify entity IDs map back
  for (size_t i = 0; i < arr->getSize(); i++)
  {
    std::uint32_t entityId = arr->getEntityIdByIdx(i);
    EXPECT_NE(entityId, UINT32_MAX);

    TestComp* byId = registry.getComponent<TestComp>(entityId);
    ASSERT_NE(byId, nullptr);
    EXPECT_EQ(byId->value, arr->getComponentByIdx(i)->value);
  }
}

// ─── Mapping consistency after remove ────────────────────────

TEST_F(ComponentArrayTest, MappingConsistentAfterRemove)
{
  addComp(10, 100);
  addComp(20, 200);
  addComp(30, 300);

  registry.removeComponent<TestComp>(10);

  auto arr = getArray();
  for (size_t i = 0; i < arr->getSize(); i++)
  {
    std::uint32_t entityId = arr->getEntityIdByIdx(i);
    EXPECT_NE(entityId, UINT32_MAX);
    EXPECT_NE(entityId, 10u) << "Removed entity should not appear in dense array";
  }
}

// ─── getEntityIdByIdx returns UINT32_MAX for invalid ─────────

TEST_F(ComponentArrayTest, EntityIdByIdxInvalidReturnsMax)
{
  EXPECT_EQ(getArray()->getEntityIdByIdx(999), UINT32_MAX);
}

// ─── removeAllComponents across multiple arrays ─────────────

struct CompA { int a = 0; };
struct CompB { int b = 0; };

TEST(ComponentRegistry, RemoveAllComponentsClearsAllArrays)
{
  ComponentRegistry reg;
  reg.registerComponent<CompA>();
  reg.registerComponent<CompB>();

  CompA ca; ca.a = 10;
  CompB cb; cb.b = 20;
  reg.addComponent<CompA>(1, std::move(ca));
  reg.addComponent<CompB>(1, std::move(cb));

  CompA ca2; ca2.a = 30;
  CompB cb2; cb2.b = 40;
  reg.addComponent<CompA>(2, std::move(ca2));
  reg.addComponent<CompB>(2, std::move(cb2));

  reg.removeAllComponents(1);

  EXPECT_EQ(reg.getComponent<CompA>(1), nullptr);
  EXPECT_EQ(reg.getComponent<CompB>(1), nullptr);

  // Entity 2 unaffected
  EXPECT_NE(reg.getComponent<CompA>(2), nullptr);
  EXPECT_EQ(reg.getComponent<CompA>(2)->a, 30);
  EXPECT_NE(reg.getComponent<CompB>(2), nullptr);
  EXPECT_EQ(reg.getComponent<CompB>(2)->b, 40);
}

TEST(ComponentRegistry, RemoveAllComponentsOnMissingEntityIsNoOp)
{
  ComponentRegistry reg;
  reg.registerComponent<CompA>();
  reg.registerComponent<CompB>();

  CompA ca; ca.a = 10;
  reg.addComponent<CompA>(1, std::move(ca));

  // Entity 99 has no components — should not crash
  reg.removeAllComponents(99);

  EXPECT_NE(reg.getComponent<CompA>(1), nullptr);
}

// ─── Remove and re-add same entity ID ─────────────────────────

TEST_F(ComponentArrayTest, RemoveAndReAddSameId)
{
  addComp(1, 100);
  registry.removeComponent<TestComp>(1);
  EXPECT_EQ(registry.getComponent<TestComp>(1), nullptr);

  addComp(1, 200);
  EXPECT_NE(registry.getComponent<TestComp>(1), nullptr);
  EXPECT_EQ(registry.getComponent<TestComp>(1)->value, 200);
}
