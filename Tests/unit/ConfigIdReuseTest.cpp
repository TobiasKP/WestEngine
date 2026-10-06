#include "support/ConfigQueueFixture.hpp"

#include <set>
#include <thread>

class ConfigIdReuse : public ConfigQueueFixture
{
};

// ─── ID reuse via freed queue ─────────────────────────────────

TEST_F(ConfigIdReuse, EntityIdsAreNeverReissued)
{
  std::uint32_t a = Config::incEntityId();
  std::uint32_t b = Config::incEntityId();
  std::uint32_t c = Config::incEntityId();
  EXPECT_LT(a, b);
  EXPECT_LT(b, c);
}

TEST_F(ConfigIdReuse, FreedUiIdIsReused)
{
  std::uint32_t original = Config::incUiId();
  Config::freedUiIds.push(original);

  std::uint32_t reused = Config::incUiId();
  EXPECT_EQ(reused, original);
}

// ─── Concurrent allocate ─────────────────────────────────────

TEST_F(ConfigIdReuse, ConcurrentAllocationProducesUniqueIds)
{
  constexpr int COUNT = 500;

  std::vector<std::uint32_t> results(COUNT);
  auto worker = [&results](int offset, int count)
  {
    for (int i = 0; i < count; i++)
    {
      results[offset + i] = Config::incEntityId();
    }
  };

  std::thread t1(worker, 0, COUNT / 2);
  std::thread t2(worker, COUNT / 2, COUNT - COUNT / 2);
  t1.join();
  t2.join();

  std::set<std::uint32_t> unique(results.begin(), results.end());
  EXPECT_EQ(unique.size(), results.size()) << "Duplicate entity IDs under concurrent allocation";
}

TEST_F(ConfigIdReuse, EmptyUiFreedQueueFallsThrough)
{
  std::uint32_t id1 = Config::incUiId();
  std::uint32_t id2 = Config::incUiId();
  EXPECT_EQ(id2, id1 + 1);
}
