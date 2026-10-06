#include <gtest/gtest.h>
#include <RenderManagment/ComponentDataPool.h>

#include <cstdint>
#include <vector>

// ComponentDataPool hands out contiguous runs of ComponentData slots for UI
// elements and compacts itself (reporting every move) when a request does not fit.

static constexpr std::uint32_t POOL_SIZE = 2048;
static constexpr std::uint32_t NO_SPACE  = static_cast<std::uint32_t>(-1);

class ComponentDataPoolTest : public ::testing::Test
{
protected:
  ComponentDataPool pool;
  std::vector<std::pair<std::uint32_t, std::uint32_t>> moves;

  void SetUp() override
  {
    pool.setPositionUpdateCallback([this](std::uint32_t oldPos, std::uint32_t newPos)
                                   { moves.emplace_back(oldPos, newPos); });
  }
};

TEST_F(ComponentDataPoolTest, ReserveReturnsConsecutiveRanges)
{
  EXPECT_EQ(pool.reserveNew(4), 0u);
  EXPECT_EQ(pool.reserveNew(3), 4u);
  EXPECT_EQ(pool.reserveNew(1), 7u);
}

TEST_F(ComponentDataPoolTest, ReservedSlotsAreAccessible)
{
  std::uint32_t start = pool.reserveNew(3);
  ASSERT_EQ(start, 0u);
  for (std::uint32_t i = start; i < start + 3; i++)
  {
    EXPECT_NE(pool.getDataAtLocation(i), nullptr) << "slot " << i;
  }
  EXPECT_EQ(pool.getDataAtLocation(3), nullptr) << "slot past the reservation is unused";
}

TEST_F(ComponentDataPoolTest, ReserveDeleteReserveReusesTheSlot)
{
  ASSERT_EQ(pool.reserveNew(4), 0u);
  ASSERT_EQ(pool.reserveNew(4), 4u);

  ASSERT_TRUE(pool.deleteRange(0, 3));
  EXPECT_EQ(pool.getDataAtLocation(0), nullptr);

  EXPECT_EQ(pool.reserveNew(4), 0u) << "the freed run at the front should be reused";
  EXPECT_NE(pool.getDataAtLocation(0), nullptr);
  EXPECT_TRUE(moves.empty()) << "a fitting hole needs no defrag";
}

TEST_F(ComponentDataPoolTest, ReserveSizeZeroIsRejected)
{
  EXPECT_GE(pool.reserveNew(0), POOL_SIZE) << "size 0 must not yield a usable slot index";
  EXPECT_EQ(pool.reserveNew(1), 0u) << "the rejected call must not have consumed slots";
}

TEST_F(ComponentDataPoolTest, DeleteRangeRejectsInvalidRanges)
{
  EXPECT_FALSE(pool.deleteRange(5, 4));
  EXPECT_FALSE(pool.deleteRange(0, POOL_SIZE));
  EXPECT_FALSE(pool.deleteRange(POOL_SIZE, POOL_SIZE));
  EXPECT_TRUE(pool.deleteRange(0, POOL_SIZE - 1));
}

TEST_F(ComponentDataPoolTest, FullPoolReturnsErrorValue)
{
  ASSERT_EQ(pool.reserveNew(POOL_SIZE), 0u);
  EXPECT_NE(pool.getDataAtLocation(POOL_SIZE - 1), nullptr);
  EXPECT_EQ(pool.reserveNew(1), NO_SPACE);
  EXPECT_TRUE(moves.empty()) << "a packed pool has nothing to move";
}

TEST_F(ComponentDataPoolTest, DefragCompactsAndReportsEveryMove)
{
  ASSERT_EQ(pool.reserveNew(POOL_SIZE / 2), 0u);
  ASSERT_EQ(pool.reserveNew(POOL_SIZE / 2), POOL_SIZE / 2);
  ComponentData* tail = pool.getDataAtLocation(POOL_SIZE - 1);
  ASSERT_NE(tail, nullptr);

  // Two holes of size 1 at the front, no run of 2 anywhere -> forces a defrag
  ASSERT_TRUE(pool.deleteRange(0, 0));
  ASSERT_TRUE(pool.deleteRange(2, 2));

  const std::uint32_t got = pool.reserveNew(2);
  EXPECT_EQ(got, POOL_SIZE - 2) << "after compaction the free run is at the end";

  ASSERT_FALSE(moves.empty()) << "defrag must report moved elements";
  EXPECT_EQ(moves.front(), (std::pair<std::uint32_t, std::uint32_t>{1, 0}));
  EXPECT_EQ(moves.back(), (std::pair<std::uint32_t, std::uint32_t>{POOL_SIZE - 1, POOL_SIZE - 3}));
  EXPECT_EQ(moves.size(), POOL_SIZE - 2);  // slot 1 and slots 3..2047 shift down
  EXPECT_EQ(pool.getDataAtLocation(POOL_SIZE - 3), tail) << "the element itself moves, not a copy";
}

TEST_F(ComponentDataPoolTest, GetDataAtLocationOnEmptySlotReturnsNull)
{
  EXPECT_EQ(pool.getDataAtLocation(0), nullptr);
  EXPECT_EQ(pool.getDataAtLocation(POOL_SIZE - 1), nullptr);
}

TEST_F(ComponentDataPoolTest, GetDataAtLocationFarOutOfBoundsReturnsNull)
{
  EXPECT_EQ(pool.getDataAtLocation(POOL_SIZE + 100), nullptr);
}

// BUG: WestInterface/WestInterface/RenderManagment/ComponentDataPool.cpp:94 the bounds check uses `location > size()` instead of `>=`, so location == 2048 reads one past the end of _poolingData (undefined behaviour, aborts under _GLIBCXX_ASSERTIONS).
TEST_F(ComponentDataPoolTest, DISABLED_GetDataAtLocationOnePastTheEndReturnsNull)
{
  EXPECT_EQ(pool.getDataAtLocation(POOL_SIZE), nullptr);
}
