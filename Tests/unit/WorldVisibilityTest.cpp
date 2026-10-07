#include <Components/LineOfSight.hpp>
#include <Entity/World.hpp>
#include <gtest/gtest.h>
#include <Interfaces/ISystem.h>

#include <set>
#include <vector>

class VisibilityMockSystem : public ISystem
{
public:
  VisibilityMockSystem()
  {
    setName("AISystem");
  }
  void update() override {}
  void updateDebuggingInfo() override {}
  void init(const std::shared_ptr<World>& w) override {}
};

class WorldVisibilityTest : public ::testing::Test
{
protected:
  World world;

  void SetUp() override
  {
    world.setCreationInformation(10, 1, glm::vec2(0, 0));
  }

  std::set<std::int32_t> visibleTiles()
  {
    std::set<std::int32_t> out;
    for (std::int32_t i = 0; i < 100; i++)
    {
      if (world.isVisible(i))
      {
        out.insert(i);
      }
    }
    return out;
  }
};

TEST_F(WorldVisibilityTest, CenterOriginMatchesReachableTiles)
{
  VisibilityMockSystem sys;
  world.updateVisibility({{55, 3}});
  std::vector<std::int32_t> reachable = world.getReachableTiles(5, 5, 3, algorithm::MANHATTAN, &sys);
  EXPECT_EQ(visibleTiles().size(), 25u);
  EXPECT_EQ(visibleTiles(), std::set<std::int32_t>(reachable.begin(), reachable.end()));
}

TEST_F(WorldVisibilityTest, TwoOriginsGiveUnion)
{
  world.updateVisibility({{0, 1}, {99, 1}});
  EXPECT_EQ(visibleTiles(), (std::set<std::int32_t>{0, 1, 10, 89, 98, 99}));
}

TEST_F(WorldVisibilityTest, CornerOriginIsClamped)
{
  world.updateVisibility({{0, 3}});
  EXPECT_EQ(visibleTiles().size(), 10u);
  EXPECT_EQ(visibleTiles(), (std::set<std::int32_t>{0, 1, 2, 3, 10, 11, 12, 20, 21, 30}));
}

TEST_F(WorldVisibilityTest, SecondCallClearsPreviousTiles)
{
  world.updateVisibility({{0, 1}});
  world.updateVisibility({{99, 1}});
  EXPECT_EQ(visibleTiles(), (std::set<std::int32_t>{89, 98, 99}));
}

TEST_F(WorldVisibilityTest, HoverAndReachableBitsArePreserved)
{
  world.setFlag(0x0001u, 0);
  world.setFlag(0x0002u, 99);
  world.updateVisibility({{0, 1}});
  world.updateVisibility({});
  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[0], 0x0001u);
  EXPECT_EQ(flags[99], 0x0002u);
}

TEST_F(WorldVisibilityTest, EmptyOriginsMeanNothingVisible)
{
  world.updateVisibility({{55, 3}});
  world.updateVisibility({});
  EXPECT_TRUE(visibleTiles().empty());
}

TEST_F(WorldVisibilityTest, OffGridOriginIsIgnored)
{
  world.updateVisibility({{-1, 3}});
  EXPECT_TRUE(visibleTiles().empty());
}

TEST_F(WorldVisibilityTest, IsVisibleOutOfRangeIsFalse)
{
  world.updateVisibility({{55, 20}});
  EXPECT_FALSE(world.isVisible(-1));
  EXPECT_FALSE(world.isVisible(100));
  EXPECT_TRUE(world.isVisible(99));
}

TEST_F(WorldVisibilityTest, BlockedTileIsNotReachable)
{
  VisibilityMockSystem sys;
  world.setFlag(0x0008u, 56);
  std::vector<std::int32_t> reachable = world.getReachableTiles(5, 5, 1, algorithm::MANHATTAN, &sys);
  EXPECT_EQ(std::set<std::int32_t>(reachable.begin(), reachable.end()), (std::set<std::int32_t>{45, 54, 55, 65}));
}

TEST_F(WorldVisibilityTest, BlockedTileKeepsOtherBitsAndIsNotFlaggedReachable)
{
  VisibilityMockSystem player;
  player.setName("PlayerControl");
  world.setFlag(0x0001u, 56);
  world.setFlag(0x0004u, 56);
  world.setFlag(0x0008u, 56);
  world.getReachableTiles(5, 5, 1, algorithm::MANHATTAN, &player);
  auto& flags = world.getFlagData();
  EXPECT_EQ(flags[56], 0x000Du);
  EXPECT_EQ(flags[54], 0x0002u);
}

TEST_F(WorldVisibilityTest, UpdateVisibilityKeepsBlockedBit)
{
  world.setFlag(0x0008u, 56);
  world.updateVisibility({{55, 1}});
  world.updateVisibility({});
  EXPECT_EQ(world.getFlagData()[56], 0x0008u);
}

TEST(LineOfSightRange, PrefersLineOfSightComponent)
{
  LineOfSight los{.range = 3};
  Movement mov{.range = 2};
  EXPECT_EQ(getLineOfSightRange(&los, &mov), 3);
}

TEST(LineOfSightRange, FallsBackToMovementRange)
{
  Movement mov{.range = 2};
  EXPECT_EQ(getLineOfSightRange(nullptr, &mov), 2);
}

TEST(LineOfSightRange, EmptyWithoutComponents)
{
  EXPECT_FALSE(getLineOfSightRange(nullptr, nullptr).has_value());
}
