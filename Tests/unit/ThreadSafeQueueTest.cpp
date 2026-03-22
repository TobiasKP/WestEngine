#include <gtest/gtest.h>
#include <ThreadSafeQueue.hpp>

#include <set>
#include <thread>

// ─── Basic operations ─────────────────────────────────────────

TEST(ThreadSafeQueue, TryPopOnEmptyReturnsNullopt)
{
  tQueue<int> q;
  EXPECT_FALSE(q.tryPop().has_value());
}

TEST(ThreadSafeQueue, PushAndTryPopRoundTrip)
{
  tQueue<int> q;
  q.push(42);

  auto result = q.tryPop();
  ASSERT_TRUE(result.has_value());
  EXPECT_EQ(result.value(), 42);
}

TEST(ThreadSafeQueue, DrainReturnsAllAndEmpties)
{
  tQueue<int> q;
  q.push(10);
  q.push(20);
  q.push(30);

  auto drained = q.drain();
  EXPECT_EQ(drained.size(), 3u);
  EXPECT_EQ(q.size(), 0u);

  std::set<int> values(drained.begin(), drained.end());
  EXPECT_TRUE(values.contains(10));
  EXPECT_TRUE(values.contains(20));
  EXPECT_TRUE(values.contains(30));
}

// ─── Concurrent push/pop ─────────────────────────────────────

TEST(ThreadSafeQueue, ConcurrentPushAndPopNoLostItems)
{
  constexpr int ITEMS = 1000;
  tQueue<int> q;

  // Producer pushes all items
  std::thread producer([&q]()
  {
    for (int i = 0; i < ITEMS; i++)
    {
      q.push(i);
    }
  });

  // Consumer tries to pop as many as possible
  std::vector<int> consumed;
  std::atomic<bool> done = false;

  std::thread consumer([&]()
  {
    while (!done || q.size() > 0)
    {
      auto val = q.tryPop();
      if (val.has_value())
      {
        consumed.push_back(val.value());
      }
    }
  });

  producer.join();
  done = true;
  consumer.join();

  // Drain any remaining
  auto remaining = q.drain();
  consumed.insert(consumed.end(), remaining.begin(), remaining.end());

  EXPECT_EQ(consumed.size(), ITEMS) << "All pushed items must be recovered";

  std::set<int> unique(consumed.begin(), consumed.end());
  EXPECT_EQ(unique.size(), ITEMS) << "No duplicates allowed";
}
