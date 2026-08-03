#include <Entity/Entity.h>
#include <gtest/gtest.h>

// ─── Move constructor ─────────────────────────────────────────

TEST(EntitySemantics, MoveConstructorTransfersState)
{
  Entity original(42);
  original.setName("Moveable");
  original.setModelGuid("test-guid-abc");
  original.setShaderId(7);
  original.destroy();
  original.debugEntity();
  original.toggleActivate(true);

  Entity moved(std::move(original));

  EXPECT_EQ(moved.getId(), 42u);
  EXPECT_EQ(moved.getName(), "Moveable");
  EXPECT_EQ(moved.getModelGuid(), "test-guid-abc");
  EXPECT_EQ(moved.getShaderId(), 7u);
  EXPECT_TRUE(moved.isDestroyed());
  EXPECT_TRUE(moved.isDebugEntity());
  EXPECT_TRUE(moved.isActiveUnit());
}

TEST(EntitySemantics, MoveConstructorResetsSourceFlags)
{
  Entity original(1);
  original.destroy();
  original.debugEntity();
  original.toggleActivate(true);

  Entity moved(std::move(original));

  EXPECT_FALSE(original.isDestroyed());
  EXPECT_FALSE(original.isDebugEntity());
  EXPECT_FALSE(original.isActiveUnit());
}

TEST(EntitySemantics, MoveConstructorTransfersModelGuid)
{
  Entity original(5);
  original.setModelGuid("model-guid-123");

  Entity moved(std::move(original));

  EXPECT_EQ(moved.getModelGuid(), "model-guid-123");
  EXPECT_TRUE(original.getModelGuid().empty());
}

// ─── Move assignment ──────────────────────────────────────────

TEST(EntitySemantics, MoveAssignmentTransfersState)
{
  Entity source(10);
  source.setName("Source");
  source.setModelGuid("guid-source");
  source.setShaderId(99);
  source.debugEntity();
  source.toggleActivate(true);

  Entity target(99);
  target = std::move(source);

  EXPECT_EQ(target.getId(), 10u);
  EXPECT_EQ(target.getName(), "Source");
  EXPECT_EQ(target.getModelGuid(), "guid-source");
  EXPECT_EQ(target.getShaderId(), 99u);
  EXPECT_TRUE(target.isDebugEntity());
  EXPECT_TRUE(target.isActiveUnit());
}

TEST(EntitySemantics, MoveAssignmentSelfIsNoop)
{
  Entity entity(7);
  entity.setName("Self");
  entity.setModelGuid("self-guid");
  entity.setShaderId(3);
  entity.debugEntity();

  Entity& ref = entity;
  entity = std::move(ref);

  EXPECT_EQ(entity.getId(), 7u);
  EXPECT_EQ(entity.getName(), "Self");
  EXPECT_EQ(entity.getModelGuid(), "self-guid");
  EXPECT_EQ(entity.getShaderId(), 3u);
  EXPECT_TRUE(entity.isDebugEntity());
}

// ─── Copy constructor ─────────────────────────────────────────

TEST(EntitySemantics, CopyConstructorCopiesAllFields)
{
  Entity original(20);
  original.setName("CopyMe");
  original.setModelGuid("copy-guid");
  original.setShaderId(15);
  original.toggleActivate(true);

  Entity copy(original);

  EXPECT_EQ(copy.getId(), 20u);
  EXPECT_EQ(copy.getName(), "CopyMe");
  EXPECT_EQ(copy.getModelGuid(), "copy-guid");
  EXPECT_EQ(copy.getShaderId(), 15u);
  EXPECT_TRUE(copy.isActiveUnit());

  // Original unchanged
  EXPECT_EQ(original.getModelGuid(), "copy-guid");
}

// ─── Copy assignment ──────────────────────────────────────────

TEST(EntitySemantics, CopyAssignmentCopiesAllFields)
{
  Entity source(30);
  source.setName("Src");
  source.setModelGuid("assign-guid");
  source.setShaderId(42);

  Entity target(1);
  target = source;

  EXPECT_EQ(target.getId(), 30u);
  EXPECT_EQ(target.getName(), "Src");
  EXPECT_EQ(target.getModelGuid(), "assign-guid");
  EXPECT_EQ(target.getShaderId(), 42u);
}
