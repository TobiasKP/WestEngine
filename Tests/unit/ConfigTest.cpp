#include "support/ConfigQueueFixture.hpp"

#include <algorithm>
#include <thread>
#include <vector>

class ConfigCounter : public ConfigQueueFixture
{
};

// The sequential counter cases live in ConfigIdReuseTest. This one is the only
// test that hammers the atomic counter itself (no freed ids involved).
TEST_F(ConfigCounter, ConcurrentIncEntityIdProducesUniqueValues)
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
