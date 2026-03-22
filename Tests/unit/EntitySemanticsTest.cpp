#include <Entity/Entity.h>
#include <gtest/gtest.h>

// ─── Move constructor ─────────────────────────────────────────

TEST(EntitySemantics, MoveConstructorTransfersState)
{
  Entity original(42);
  original.setName("Moveable");
  original.destroy();
  original.debugEntity();

  Entity moved(std::move(original));

  EXPECT_EQ(moved.getId(), 42u);
  EXPECT_EQ(moved.getName(), "Moveable");
  EXPECT_TRUE(moved.isDestroyed());
  EXPECT_TRUE(moved.isDebugEntity());
}

TEST(EntitySemantics, MoveConstructorResetsSourceFlags)
{
  Entity original(1);
  original.destroy();
  original.debugEntity();

  Entity moved(std::move(original));

  EXPECT_FALSE(original.isDestroyed());
  EXPECT_FALSE(original.isDebugEntity());
}

// ─── Move assignment ──────────────────────────────────────────

TEST(EntitySemantics, MoveAssignmentTransfersState)
{
  Entity source(10);
  source.setName("Source");
  source.debugEntity();

  Entity target(99);
  target = std::move(source);

  EXPECT_EQ(target.getId(), 10u);
  EXPECT_EQ(target.getName(), "Source");
  EXPECT_TRUE(target.isDebugEntity());
}

TEST(EntitySemantics, MoveAssignmentSelfIsNoop)
{
  Entity entity(7);
  entity.setName("Self");
  entity.debugEntity();

  Entity& ref = entity;
  entity = std::move(ref);

  EXPECT_EQ(entity.getId(), 7u);
  EXPECT_EQ(entity.getName(), "Self");
  EXPECT_TRUE(entity.isDebugEntity());
}

