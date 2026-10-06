#pragma once

// Shared world/registry setup for the ECS stress tests (unit/ECSStressTest.cpp)
// and the timing tests (perf/ECSPerfTest.cpp, ctest label "perf").

#include <chrono>
#include <Components/ComponentRegistry.hpp>
#include <Entity/World.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <gtest/gtest.h>
#include <Interfaces/ISystem.h>
#include <random>

namespace ecs_stress
{
class StubSystem : public ISystem
{
public:
  StubSystem(std::string_view name)
  {
    setName(name);
  }
  void update() override {}
  void updateDebuggingInfo() override {}
  void init(const std::shared_ptr<World>&) override {}
};
}  // namespace ecs_stress

using ecs_stress::StubSystem;

// ─── ECS Stress Test ──────────────────────────────────────────
//
// Simulates a game tick for 200 entities:
//   - ComponentRegistry lookups (Position, Movement, Appearance)
//   - World tile queries (calculateIndex, getReachableTiles)
//   - Position mutation (movement lerp)
//   - Appearance mutation (emissive highlight)
//   - World flag operations (setFlag, clearFlag)
//   - Combat: 20 entities fire projectiles, hit resolution, damage
//   - Entity destruction for killed targets
//
// Target: < 8ms on dev machine to leave headroom for rendering.
// ──────────────────────────────────────────────────────────────

inline constexpr std::uint32_t ENTITY_COUNT    = 200;
inline constexpr std::uint32_t MOVING_COUNT    = 50;
inline constexpr std::uint32_t ATTACKING_COUNT = 20;
inline constexpr std::uint32_t GRID_SIZE       = 50;
inline constexpr double TARGET_MS              = 8.0;

class ECSStressTest : public ::testing::Test
{
protected:
  ComponentRegistry registry;
  World world;
  StubSystem playerStub{"PlayerControl"};
  StubSystem otherStub{"Other"};
  std::mt19937 rng{42};  // deterministic seed

  void SetUp() override
  {
    world.setCreationInformation(GRID_SIZE, 1, glm::vec2(0, 0));

    registry.registerComponent<Position>();
    registry.registerComponent<Movement>();
    registry.registerComponent<Appearance>();
    registry.registerComponent<Control>();
    registry.registerComponent<Health>();
    registry.registerComponent<Equipment>();
    registry.registerComponent<Projectile>();

    std::uniform_real_distribution<float> posDist(1.0f, (float)(GRID_SIZE - 1));
    std::uniform_int_distribution<std::int32_t> rangeDist(1, 4);

    for (std::uint32_t i = 1; i <= ENTITY_COUNT; i++)
    {
      Position pos;
      pos.position = glm::vec3(posDist(rng), 0.0f, posDist(rng));
      pos.dirty.store(true);
      registry.addComponent<Position>(i, std::move(pos));

      Movement mov;
      mov.range = rangeDist(rng);
      mov.a     = algorithm::MANHATTAN;
      registry.addComponent<Movement>(i, std::move(mov));

      Appearance app;
      registry.addComponent<Appearance>(i, std::move(app));

      Health h;
      h.max     = 100;
      h.current = 100;
      registry.addComponent<Health>(i, std::move(h));

      Equipment eq;
      eq.primary   = {1, 1, 25, 3, 85.0f};
      eq.secondary = {2, 2, 10, 5, 95.0f};
      eq.active    = 1;
      registry.addComponent<Equipment>(i, std::move(eq));
    }

    // One player-controlled entity
    Control ctrl;
    ctrl.entityId = 1;
    registry.addComponent<Control>(1, std::move(ctrl));

    // Place all entities on the world grid
    auto posArr = registry.getComponentArray<Position>();
    for (size_t i = 0; i < posArr->getSize(); i++)
    {
      std::uint32_t id = posArr->getEntityIdByIdx(i);
      Position* p      = posArr->getComponentByIdx(i);
      world.addEntityIdToIdx(p->position.x, p->position.z, id);
    }

    // Pre-assign destinations to MOVING_COUNT entities (simulate mid-movement)
    std::uniform_real_distribution<float> destDist(1.0f, (float)(GRID_SIZE - 1));
    auto movArr = registry.getComponentArray<Movement>();
    for (size_t i = 0; i < MOVING_COUNT && i < movArr->getSize(); i++)
    {
      movArr->getComponentByIdx(i)->destination = glm::vec3(destDist(rng), 0.0f, destDist(rng));

      // Mark position dirty so transform rebuild fires
      std::uint32_t id = movArr->getEntityIdByIdx(i);
      Position* pos    = registry.getComponent<Position>(id);
      pos->dirty.store(true);
    }

    // Clear dirty on non-moving entities (they've already been rendered)
    for (size_t i = MOVING_COUNT; i < posArr->getSize(); i++)
    {
      posArr->getComponentByIdx(i)->dirty.store(false);
    }
  }
};
