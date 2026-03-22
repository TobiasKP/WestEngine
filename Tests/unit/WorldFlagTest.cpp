#include <Entity/World.hpp>
#include <gtest/gtest.h>

class WorldFlagTest : public ::testing::Test
{
protected:
  World world;

  void SetUp() override
  {
    world.setCreationInformation(10, 1, glm::vec2(0, 0));
  }
};

// ─── setFlag / clearFlag ──────────────────────────────────────

TEST_F(WorldFlagTest, SetFlagSetsCorrectBit)
{
  world.setFlag(0x0002u, 5);

  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[5] & 0x0002u, 0x0002u);
}

TEST_F(WorldFlagTest, SetFlagDoesNotAffectOtherTiles)
{
  world.setFlag(0x0002u, 5);

  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[0], 0u);
  EXPECT_EQ(flags[4], 0u);
  EXPECT_EQ(flags[6], 0u);
}

TEST_F(WorldFlagTest, ClearFlagRemovesBitFromAllTiles)
{
  world.setFlag(0x0002u, 3);
  world.setFlag(0x0002u, 7);

  world.clearFlag(0x0002u);

  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[3] & 0x0002u, 0u);
  EXPECT_EQ(flags[7] & 0x0002u, 0u);
}

TEST_F(WorldFlagTest, ClearFlagPreservesOtherBits)
{
  world.setFlag(0x0001u, 5);
  world.setFlag(0x0002u, 5);

  world.clearFlag(0x0002u);

  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[5] & 0x0001u, 0x0001u);
  EXPECT_EQ(flags[5] & 0x0002u, 0u);
}

TEST_F(WorldFlagTest, MultipleFlagsOnSameTile)
{
  world.setFlag(0x0001u, 5);
  world.setFlag(0x0002u, 5);
  world.setFlag(0x0004u, 5);

  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[5], 0x0007u);
}

// ─── isDirty ──────────────────────────────────────────────────

TEST_F(WorldFlagTest, DirtyAfterSetFlag)
{
  world.resetDirty();
  EXPECT_FALSE(world.isDirty());

  world.setFlag(0x0001u, 0);
  EXPECT_TRUE(world.isDirty());
}

TEST_F(WorldFlagTest, DirtyAfterClearFlag)
{
  world.resetDirty();
  world.clearFlag(0x0001u);
  EXPECT_TRUE(world.isDirty());
}

TEST_F(WorldFlagTest, ResetDirtyClearsDirty)
{
  world.setFlag(0x0001u, 0);
  EXPECT_TRUE(world.isDirty());
  world.resetDirty();
  EXPECT_FALSE(world.isDirty());
}

// ─── addEntityIdToIdx / getEntityByIdx ────────────────────────

TEST_F(WorldFlagTest, AddAndGetEntityByTile)
{
  world.addEntityIdToIdx(3.5f, 4.5f, 42);
  std::int32_t idx = world.calculateIndex(3.5, 4.5);
  EXPECT_EQ(world.getEntityByIdx(idx), 42u);
}

TEST_F(WorldFlagTest, GetEntityByIdxReturnsZeroForEmpty)
{
  EXPECT_EQ(world.getEntityByIdx(0), 0u);
}

TEST_F(WorldFlagTest, EntityMovesWhenReAdded)
{
  world.addEntityIdToIdx(1.5f, 1.5f, 99);
  std::int32_t oldIdx = world.calculateIndex(1.5, 1.5);

  // Move entity to new position
  world.addEntityIdToIdx(5.5f, 5.5f, 99);
  std::int32_t newIdx = world.calculateIndex(5.5, 5.5);

  EXPECT_EQ(world.getEntityByIdx(newIdx), 99u);
  EXPECT_EQ(world.getEntityByIdx(oldIdx), 0u) << "Old tile should be cleared after move";
}

TEST_F(WorldFlagTest, AddEntityOutOfBoundsIsIgnored)
{
  world.addEntityIdToIdx(-1.0f, -1.0f, 77);
  // Should not crash, entity not placed
  EXPECT_EQ(world.getEntityByIdx(0), 0u);
}
