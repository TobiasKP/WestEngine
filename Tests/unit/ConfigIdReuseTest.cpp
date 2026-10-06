#include "support/ConfigQueueFixture.hpp"

#include <set>
#include <thread>

class ConfigIdReuse : public ConfigQueueFixture
{
};

// ─── ID reuse via freed queue ─────────────────────────────────

TEST_F(ConfigIdReuse, FreedEntityIdIsReused)
{
  std::uint32_t original = Config::incEntityId();
  Config::freedEntityIds.push(original);

  std::uint32_t reused = Config::incEntityId();
  EXPECT_EQ(reused, original);
}

TEST_F(ConfigIdReuse, FreedUiIdIsReused)
{
  std::uint32_t original = Config::incUiId();
  Config::freedUiIds.push(original);

  std::uint32_t reused = Config::incUiId();
  EXPECT_EQ(reused, original);
}

TEST_F(ConfigIdReuse, FreedIdsAreExhaustedBeforeCounter)
{
  std::uint32_t a = Config::incEntityId();
  std::uint32_t b = Config::incEntityId();

  Config::freedEntityIds.push(a);
  Config::freedEntityIds.push(b);

  std::uint32_t r1 = Config::incEntityId();
  std::uint32_t r2 = Config::incEntityId();

  // Both freed IDs should come back (order is LIFO from vector)
  std::set<std::uint32_t> freed   = {a, b};
  std::set<std::uint32_t> reused  = {r1, r2};
  EXPECT_EQ(freed, reused);

  // Next call should come from the atomic counter (fresh ID)
  std::uint32_t fresh = Config::incEntityId();
  EXPECT_FALSE(freed.contains(fresh));
}

TEST_F(ConfigIdReuse, EmptyFreedQueueFallsThrough)
{
  // The fixture drained the freed queues, so ids come straight from the counter
  std::uint32_t id1 = Config::incEntityId();
  std::uint32_t id2 = Config::incEntityId();
  EXPECT_EQ(id2, id1 + 1);
}

// ─── Concurrent free + allocate ──────────────────────────────

TEST_F(ConfigIdReuse, ConcurrentFreeAndAllocateProducesNoLostIds)
{
  constexpr int COUNT = 500;

  // Generate IDs, then free them all
  std::vector<std::uint32_t> ids(COUNT);
  for (int i = 0; i < COUNT; i++)
  {
    ids[i] = Config::incEntityId();
  }
  for (auto id : ids)
  {
    Config::freedEntityIds.push(id);
  }

  // Concurrently reclaim them
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

  // All returned IDs must be unique
  std::set<std::uint32_t> unique(results.begin(), results.end());
  EXPECT_EQ(unique.size(), results.size()) << "Duplicate IDs under concurrent free+alloc";
}

TEST_F(ConfigIdReuse, EmptyUiFreedQueueFallsThrough)
{
  std::uint32_t id1 = Config::incUiId();
  std::uint32_t id2 = Config::incUiId();
  EXPECT_EQ(id2, id1 + 1);
}
