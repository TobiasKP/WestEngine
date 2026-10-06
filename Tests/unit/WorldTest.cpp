#include <Entity/World.hpp>
#include <gtest/gtest.h>
#include <Interfaces/ISystem.h>

#include <set>
#include <vector>

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
  void init(const std::shared_ptr<World>& w) override {}
};

// Indices are row * 10 + column on the 10x10 test grid.
static std::set<std::int32_t> asSet(const std::vector<std::int32_t>& v)
{
  return std::set<std::int32_t>(v.begin(), v.end());
}

TEST_F(WorldTest, GetReachableTilesRange1FromCenter)
{
  MockSystem sys;
  // Center tile (row 5, col 5), range 1 -> diamond: center + 4 neighbours
  auto tiles = world.getReachableTiles(5, 5, 1, algorithm::MANHATTAN, &sys);
  EXPECT_EQ(tiles.size(), 5u) << "no duplicates expected";
  EXPECT_EQ(asSet(tiles), (std::set<std::int32_t>{45, 54, 55, 56, 65}));
}

TEST_F(WorldTest, GetReachableTilesRange2FromCenter)
{
  MockSystem sys;
  auto tiles = world.getReachableTiles(5, 5, 2, algorithm::MANHATTAN, &sys);
  EXPECT_EQ(tiles.size(), 13u) << "no duplicates expected";
  EXPECT_EQ(asSet(tiles), (std::set<std::int32_t>{35, 44, 45, 46, 53, 54, 55, 56, 57, 64, 65, 66, 75}));
}

TEST_F(WorldTest, GetReachableTilesAtCornerIsClamped)
{
  MockSystem sys;
  // Corner (0,0), range 2 -> only the in-grid quarter of the diamond
  auto tiles = world.getReachableTiles(0, 0, 2, algorithm::MANHATTAN, &sys);
  EXPECT_EQ(tiles.size(), 6u);
  EXPECT_EQ(asSet(tiles), (std::set<std::int32_t>{0, 1, 2, 10, 11, 20}));
}

TEST_F(WorldTest, GetReachableTilesAtFarCornerIsClamped)
{
  MockSystem sys;
  auto tiles = world.getReachableTiles(9, 9, 1, algorithm::MANHATTAN, &sys);
  EXPECT_EQ(asSet(tiles), (std::set<std::int32_t>{89, 98, 99}));
}

// ─── getReachableTiles flag side effect ────────────────────

class NamedSystem : public ISystem
{
public:
  explicit NamedSystem(std::string_view name)
  {
    setName(name);
  }
  void update() override {}
  void updateDebuggingInfo() override {}
  void init(const std::shared_ptr<World>& w) override {}
};

TEST_F(WorldTest, GetReachableTilesFromPlayerControlFlagsExactlyTheResult)
{
  NamedSystem player("PlayerControl");
  auto tiles      = world.getReachableTiles(5, 5, 1, algorithm::MANHATTAN, &player);
  auto reachable  = asSet(tiles);
  auto& flags     = world.getFlagData();
  for (std::int32_t i = 0; i < static_cast<std::int32_t>(flags.size()); i++)
  {
    const bool flagged = (flags[i] & 0x0002u) != 0;
    EXPECT_EQ(flagged, reachable.contains(i)) << "tile " << i;
  }
  EXPECT_TRUE(world.isDirty());
}

TEST_F(WorldTest, GetReachableTilesFromOtherSystemsLeavesFlagsUntouched)
{
  NamedSystem ai("AISystem");
  world.resetDirty();
  auto tiles = world.getReachableTiles(5, 5, 2, algorithm::MANHATTAN, &ai);
  EXPECT_EQ(tiles.size(), 13u);
  for (std::uint32_t f : world.getFlagData())
  {
    EXPECT_EQ(f, 0u);
  }
  EXPECT_FALSE(world.isDirty());
}

// ─── worldPosToTile (hover flag) ───────────────────────────

TEST_F(WorldTest, WorldPosToTileReturnsIndexAndSetsHoverFlag)
{
  EXPECT_EQ(world.worldPosToTile(3.5, 4.5), 43);
  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[43] & 0x0001u, 0x0001u);
}

TEST_F(WorldTest, WorldPosToTileMovesHoverFlag)
{
  world.worldPosToTile(3.5, 4.5);
  EXPECT_EQ(world.worldPosToTile(6.5, 4.5), 46);

  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[43] & 0x0001u, 0u) << "previous hover tile must be cleared";
  EXPECT_EQ(flags[46] & 0x0001u, 0x0001u);
}

TEST_F(WorldTest, WorldPosToTileSameTileTwiceKeepsFlag)
{
  world.worldPosToTile(3.2, 4.2);
  EXPECT_EQ(world.worldPosToTile(3.8, 4.8), 43);
  EXPECT_EQ(world.getFlagData()[43] & 0x0001u, 0x0001u);
}

TEST_F(WorldTest, WorldPosToTileOffGridClearsHoverFlag)
{
  world.worldPosToTile(3.5, 4.5);
  EXPECT_EQ(world.worldPosToTile(-2.0, 4.5), -1);
  for (std::uint32_t f : world.getFlagData())
  {
    EXPECT_EQ(f & 0x0001u, 0u);
  }
}

TEST_F(WorldTest, WorldPosToTilePreservesOtherFlagBits)
{
  world.setFlag(0x0002u, 43);
  world.worldPosToTile(3.5, 4.5);
  world.worldPosToTile(6.5, 4.5);
  EXPECT_EQ(world.getFlagData()[43], 0x0002u);
}

// ─── getEntitiesInRange ────────────────────────────────────

TEST_F(WorldTest, GetEntitiesInRangeExcludesSelfAndOutOfRange)
{
  world.addEntityIdToIdx(5.5f, 5.5f, 1);  // tile 55, the caller
  world.addEntityIdToIdx(6.5f, 5.5f, 2);  // tile 56, distance 1
  world.addEntityIdToIdx(7.5f, 5.5f, 3);  // tile 57, distance 2
  world.addEntityIdToIdx(9.5f, 9.5f, 4);  // tile 99, far away

  EXPECT_EQ(asSet(world.getEntitiesInRange(5, 5, 1, algorithm::MANHATTAN, 1)), (std::set<std::int32_t>{56}));
  EXPECT_EQ(asSet(world.getEntitiesInRange(5, 5, 2, algorithm::MANHATTAN, 1)), (std::set<std::int32_t>{56, 57}));
}

TEST_F(WorldTest, GetEntitiesInRangeEmptyGridReturnsNothing)
{
  EXPECT_TRUE(world.getEntitiesInRange(5, 5, 3, algorithm::MANHATTAN, 1).empty());
}

TEST_F(WorldTest, GetEntitiesInRangeReturnsTileIndicesThatMapBackToEntities)
{
  world.addEntityIdToIdx(5.5f, 5.5f, 10);
  world.addEntityIdToIdx(5.5f, 4.5f, 20);  // tile 45

  auto tiles = world.getEntitiesInRange(5, 5, 1, algorithm::MANHATTAN, 10);
  ASSERT_EQ(tiles.size(), 1u);
  EXPECT_EQ(tiles[0], 45);
  EXPECT_EQ(world.getEntityByIdx(tiles[0]), 20u);
}

// ─── Two entities on one tile ──────────────────────────────

// BUG: WestEngine/Core/Entity/World.cpp:85-86 addEntityIdToIdx overwrites an occupied tile without dropping the previous occupant's reverse entry, so removing (or moving) the displaced entity later erases the current occupant from the grid.
TEST_F(WorldTest, DISABLED_SecondEntityOnOccupiedTileKeepsGridConsistent)
{
  world.addEntityIdToIdx(5.5f, 5.5f, 1);
  world.addEntityIdToIdx(5.5f, 5.5f, 2);

  // Whichever policy applies (reject the newcomer or replace the occupant),
  // removing the entity that is NOT on the tile must leave the tile alone.
  const std::uint32_t occupant = world.getEntityByIdx(55);
  ASSERT_TRUE(occupant == 1u || occupant == 2u);
  const std::uint32_t other = occupant == 1u ? 2u : 1u;

  world.removeEntityFromGrid(other);
  EXPECT_EQ(world.getEntityByIdx(55), occupant);

  world.updateEntityIdToIdx(2.5f, 2.5f, other);
  EXPECT_EQ(world.getEntityByIdx(55), occupant);
}
