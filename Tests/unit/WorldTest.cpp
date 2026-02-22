#include <gtest/gtest.h>
#include <Entity/World.hpp>
#include <Interfaces/ISystem.h>

class WorldTest : public ::testing::Test
{
protected:
  World world;

  void SetUp() override
  {
    world.setCreationInformation(10, 1, glm::vec2(0, 0));
  }
};

// ─── calculateIndex ────────────────────────────────────────

TEST_F(WorldTest, CalculateIndexReturnsCorrectForOrigin)
{
  EXPECT_EQ(world.calculateIndex(0.5, 0.5), 0);
}

TEST_F(WorldTest, CalculateIndexReturnsCorrectForCenter)
{
  // tile (5, 3) → floor(5.5)=5 + floor(3.2)*10=30 → 35
  EXPECT_EQ(world.calculateIndex(5.5, 3.2), 35);
}

TEST_F(WorldTest, CalculateIndexReturnsNegativeForOutOfBounds)
{
  EXPECT_EQ(world.calculateIndex(-1.0, 0.0), -1);
  EXPECT_EQ(world.calculateIndex(0.0, -1.0), -1);
  EXPECT_EQ(world.calculateIndex(10.0, 0.0), -1);
  EXPECT_EQ(world.calculateIndex(0.0, 10.0), -1);
}

TEST_F(WorldTest, CalculateIndexEdgeTiles)
{
  // Last valid tile: (9, 9) → 9 + 9*10 = 99
  EXPECT_EQ(world.calculateIndex(9.9, 9.9), 99);
  // First tile
  EXPECT_EQ(world.calculateIndex(0.0, 0.0), 0);
}

// ─── tileToWorldPos ────────────────────────────────────────

TEST_F(WorldTest, TileToWorldPosReturnsCenter)
{
  auto pos = world.tileToWorldPos(0);
  ASSERT_TRUE(pos.has_value());
  EXPECT_FLOAT_EQ(pos->x, 0.5f);
  EXPECT_FLOAT_EQ(pos->y, 0.0f);
  EXPECT_FLOAT_EQ(pos->z, 0.5f);
}

TEST_F(WorldTest, TileToWorldPosRoundTrip)
{
  // Index 35 → world pos → back to index
  auto pos = world.tileToWorldPos(35);
  ASSERT_TRUE(pos.has_value());
  std::int32_t idx = world.calculateIndex(pos->x, pos->z);
  EXPECT_EQ(idx, 35);
}

TEST_F(WorldTest, TileToWorldPosInvalidReturnsEmpty)
{
  auto pos = world.tileToWorldPos(-1);
  EXPECT_FALSE(pos.has_value());
}

// ─── getReachableTiles ─────────────────────────────────────

class MockSystem : public ISystem
{
public:
  MockSystem()
  {
    setName("PlayerControl");
  }
  void update() override {}
  void updateDebuggingInfo() override {}
  void init() override {}
};

TEST_F(WorldTest, GetReachableTilesRange1FromCenter)
{
  MockSystem sys;
  // Center tile (5,5), range 1 → 5 tiles (diamond: center + 4 neighbors)
  auto tiles = world.getReachableTiles(5, 5, 1, algorithm::MANHATTAN, &sys);
  EXPECT_EQ(tiles.size(), 5);
}

TEST_F(WorldTest, GetReachableTilesRange2FromCenter)
{
  MockSystem sys;
  // Center tile (5,5), range 2 → 13 tiles
  auto tiles = world.getReachableTiles(5, 5, 2, algorithm::MANHATTAN, &sys);
  EXPECT_EQ(tiles.size(), 13);
}

TEST_F(WorldTest, GetReachableTilesAtCornerIsClamped)
{
  MockSystem sys;
  // Corner tile (0,0), range 2 → should be clamped, fewer tiles
  auto tiles = world.getReachableTiles(0, 0, 2, algorithm::MANHATTAN, &sys);
  EXPECT_LT(tiles.size(), 13u) << "Corner should have fewer reachable tiles than center";
  EXPECT_GE(tiles.size(), 3u);
}
