#include <gtest/gtest.h>
#include <Config.h>

TEST(Config, IncEntityIdReturnsSequentialValues)
{
  std::uint32_t first  = Config::incEntityId();
  std::uint32_t second = Config::incEntityId();
  std::uint32_t third  = Config::incEntityId();

  EXPECT_EQ(second, first + 1);
  EXPECT_EQ(third, second + 1);
}

TEST(Config, IncUiIdReturnsSequentialValues)
{
  std::uint32_t first  = Config::incUiId();
  std::uint32_t second = Config::incUiId();
  std::uint32_t third  = Config::incUiId();

  EXPECT_EQ(second, first + 1);
  EXPECT_EQ(third, second + 1);
}

TEST(Config, ConcurrentIncEntityIdProducesUniqueValues)
{
  constexpr int COUNT = 1000;
  std::vector<std::uint32_t> results(COUNT * 2);

  auto increment = [&results](int offset)
  {
    for (int i = 0; i < COUNT; i++)
    {
      results[offset + i] = Config::incEntityId();
    }
  };

  std::thread t1(increment, 0);
  std::thread t2(increment, COUNT);
  t1.join();
  t2.join();

  std::sort(results.begin(), results.end());
  auto it = std::unique(results.begin(), results.end());
  EXPECT_EQ(it, results.end()) << "Duplicate IDs found under concurrent access";
}
