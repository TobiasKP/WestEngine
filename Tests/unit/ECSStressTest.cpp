#include "support/ECSStressFixture.hpp"

// Timing tests for this fixture live in perf/ECSPerfTest.cpp (ctest label "perf").

// ─── Combat: mass damage and entity removal ──────────────────

TEST_F(ECSStressTest, MassCombatRemovalStability)
{
  // All 200 entities shoot at entity 100, killing it many times over
  std::uint32_t target = 100;
  std::vector<std::uint32_t> toRemove;

  for (std::uint32_t attacker = 1; attacker <= ENTITY_COUNT; attacker++)
  {
    if (attacker == target)
    {
      continue;
    }

    Health* h = registry.getComponent<Health>(target);
    if (h == nullptr)
    {
      break;  // already removed
    }

    Equipment* eq  = registry.getComponent<Equipment>(attacker);
    Weapon* active = &eq->primary;

    h->current -= active->dmg;
    if (h->current <= 0)
    {
      toRemove.push_back(target);
      break;
    }
  }

  for (std::uint32_t id : toRemove)
  {
    registry.removeAllComponents(id);
  }

  // Verify target is fully removed
  EXPECT_EQ(registry.getComponent<Position>(target), nullptr);
  EXPECT_EQ(registry.getComponent<Health>(target), nullptr);
  EXPECT_EQ(registry.getComponent<Equipment>(target), nullptr);
  EXPECT_EQ(registry.getComponent<Appearance>(target), nullptr);
  EXPECT_EQ(registry.getComponent<Movement>(target), nullptr);

  // Verify other entities still intact
  EXPECT_NE(registry.getComponent<Position>(1), nullptr);
  EXPECT_NE(registry.getComponent<Health>(1), nullptr);
  EXPECT_NE(registry.getComponent<Position>(ENTITY_COUNT), nullptr);
}
