#include "../unit/support/ECSStressFixture.hpp"

#include <iostream>

// Wall-clock budget tests. They depend on machine load, so they are not part of
// the default ctest run, see Tests/CMakeLists.txt (WEST_PERF_TESTS, label "perf").
using ECSPerfTest = ECSStressTest;

// ─── Simulate one full systems tick ──────────────────────────

TEST_F(ECSPerfTest, FullSystemsTickUnder8ms)
{
  auto start = std::chrono::steady_clock::now();

  // --- PlayerControl-like pass ---
  auto controlArr = registry.getComponentArray<Control>();
  for (size_t i = 0; i < controlArr->getSize(); i++)
  {
    std::uint32_t id = controlArr->getComponents()[i].entityId;
    Movement* mov    = registry.getComponent<Movement>(id);
    Position* pos    = registry.getComponent<Position>(id);

    std::int32_t tileIdx = world.calculateIndex(pos->position.x, pos->position.z);
    std::int32_t col     = tileIdx % GRID_SIZE;
    std::int32_t row     = tileIdx / GRID_SIZE;
    auto reachable       = world.getReachableTiles(row, col, mov->range, mov->a, &playerStub);
    mov->reachableTiles  = reachable;
  }

  // --- MovementSystem-like pass ---
  auto movArr = registry.getComponentArray<Movement>();
  for (size_t i = 0; i < movArr->getSize(); i++)
  {
    Movement* mov    = movArr->getComponentByIdx(i);
    std::uint32_t id = movArr->getEntityIdByIdx(i);
    if (mov->destination.has_value())
    {
      Position* pos = registry.getComponent<Position>(id);
      glm::vec3 dir = *mov->destination - pos->position;
      float len2    = glm::dot(dir, dir);
      if (len2 > 0.025f)
      {
        dir            = glm::normalize(dir) * 2.5f * 0.016f;  // speed * delta
        pos->position += dir;
      }
      else
      {
        pos->position = *mov->destination;
        mov->destination.reset();
      }
      pos->dirty.store(true);
      world.updateEntityIdToIdx(pos->position.x, pos->position.z, id);
    }
  }

  // --- ProjectileSystem-like pass: attack resolution ---
  // 20 entities fire at random targets
  std::vector<std::uint32_t> toRemove;
  std::uniform_int_distribution<std::uint32_t> targetDist(1, ENTITY_COUNT);
  auto posArr = registry.getComponentArray<Position>();

  for (std::uint32_t attacker = 1; attacker <= ATTACKING_COUNT; attacker++)
  {
    std::uint32_t target = targetDist(rng);
    if (target == attacker)
    {
      continue;
    }

    Equipment* eq  = registry.getComponent<Equipment>(attacker);
    Weapon* active = (eq->active == 1) ? &eq->primary : &eq->secondary;

    Position* aPosComp = registry.getComponent<Position>(attacker);
    Position* tPosComp = registry.getComponent<Position>(target);

    std::int32_t aTile     = world.calculateIndex(aPosComp->position.x, aPosComp->position.z);
    std::int32_t tTile     = world.calculateIndex(tPosComp->position.x, tPosComp->position.z);
    std::int32_t dimension = world.getGridSize();
    std::int32_t dx        = std::abs(aTile % dimension - tTile % dimension);
    std::int32_t dz        = std::abs(aTile / dimension - tTile / dimension);
    std::int32_t distance  = std::max(dx, dz);

    // Hit resolution (simulates Lua side)
    float accuracy = active->accuracy;
    if (active->range < (std::uint32_t)distance)
    {
      accuracy -= (distance - active->range) * 10.0f;
    }

    std::uniform_int_distribution<int> hitRoll(0, 100);
    bool hit = hitRoll(rng) <= (int)accuracy;

    // Spawn projectile component
    Projectile proj;
    proj.speed           = 2;
    proj.damage          = active->dmg;
    proj.destination     = target;
    proj.hit             = hit;
    std::uint32_t projId = ENTITY_COUNT + attacker;
    Position projPos;
    projPos.position = aPosComp->position;
    projPos.dirty.store(true);
    registry.addComponent<Position>(projId, std::move(projPos));
    registry.addComponent<Projectile>(projId, std::move(proj));
  }

  // --- ProjectileSystem-like pass: travel and impact ---
  auto projectiles = registry.getComponentArray<Projectile>();
  size_t projSize  = projectiles->getSize();
  size_t current   = 0;
  for (auto& p : projectiles->getComponents())
  {
    if (current >= projSize)
    {
      break;
    }
    std::uint32_t id  = projectiles->getEntityIdByIdx(current);
    Position* projPos = registry.getComponent<Position>(id);
    Position* tgtPos  = registry.getComponent<Position>(p.destination);

    if (tgtPos != nullptr)
    {
      // Simulate instant arrival for stress test
      projPos->position = tgtPos->position;

      if (p.hit)
      {
        Health* h = registry.getComponent<Health>(p.destination);
        if (h != nullptr)
        {
          h->current -= p.damage;
          if (h->current <= 0)
          {
            toRemove.push_back(p.destination);
          }
        }
      }
      toRemove.push_back(id);
    }
    current++;
  }

  // --- Deferred entity removal ---
  for (std::uint32_t id : toRemove)
  {
    registry.removeAllComponents(id);
  }

  // --- PositionalSystem-like pass (hover highlight via Appearance) ---
  posArr = registry.getComponentArray<Position>();
  for (size_t i = 0; i < posArr->getSize(); i++)
  {
    Position* pos    = posArr->getComponentByIdx(i);
    std::uint32_t id = posArr->getEntityIdByIdx(i);

    std::int32_t idx     = world.calculateIndex(pos->position.x, pos->position.z);
    std::uint32_t tileId = world.getEntityByIdx(idx);
    if (tileId > 0)
    {
      Appearance* app = registry.getComponent<Appearance>(tileId);
      if (app != nullptr)
      {
        app->emissiveOverride = glm::vec3(0.0f, 0.5f, 0.5f);
      }
    }
  }

  // --- Dirty flag pass (what RenderManager checks) ---
  for (size_t i = 0; i < posArr->getSize(); i++)
  {
    Position* pos = posArr->getComponentByIdx(i);
    if (pos->dirty.load())
    {
      pos->transform = glm::translate(glm::mat4(1.0f), pos->position);
      pos->dirty.store(false);
    }
  }

  // --- World flag clear (end of frame) ---
  world.clearFlag(0x0002u);

  auto end       = std::chrono::steady_clock::now();
  double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

  std::cout << "[  PERF   ] Full systems tick (" << ENTITY_COUNT << " entities, " << ATTACKING_COUNT
            << " attacks): " << elapsed << " ms" << std::endl;

  EXPECT_LT(elapsed, TARGET_MS) << "Systems tick exceeded " << TARGET_MS << "ms budget";
}

// ─── Component lookup scalability ────────────────────────────

TEST_F(ECSPerfTest, ComponentLookupScalability)
{
  auto start = std::chrono::steady_clock::now();

  // Simulate 200 entities × 5 component lookups × 60 fps = one second of lookups
  for (int frame = 0; frame < 60; frame++)
  {
    for (std::uint32_t id = 1; id <= ENTITY_COUNT; id++)
    {
      volatile Position* p    = registry.getComponent<Position>(id);
      volatile Movement* m    = registry.getComponent<Movement>(id);
      volatile Appearance* ap = registry.getComponent<Appearance>(id);
      volatile Health* h      = registry.getComponent<Health>(id);
      volatile Equipment* eq  = registry.getComponent<Equipment>(id);
      (void)p;
      (void)m;
      (void)ap;
      (void)h;
      (void)eq;
    }
  }

  auto end       = std::chrono::steady_clock::now();
  double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

  std::cout << "[  PERF   ] 60 frames × " << ENTITY_COUNT << " entities × 5 lookups: " << elapsed
            << " ms (budget: " << 60 * TARGET_MS << " ms)" << std::endl;

  EXPECT_LT(elapsed, 60 * TARGET_MS);
}

// ─── World query scalability ─────────────────────────────────

TEST_F(ECSPerfTest, WorldQueryScalability)
{
  auto start = std::chrono::steady_clock::now();

  auto posArr = registry.getComponentArray<Position>();
  for (size_t i = 0; i < posArr->getSize(); i++)
  {
    Position* pos = posArr->getComponentByIdx(i);
    Movement* mov = registry.getComponent<Movement>(posArr->getEntityIdByIdx(i));

    std::int32_t idx = world.calculateIndex(pos->position.x, pos->position.z);
    if (idx >= 0)
    {
      std::int32_t col = idx % GRID_SIZE;
      std::int32_t row = idx / GRID_SIZE;
      auto tiles       = world.getReachableTiles(row, col, mov->range, mov->a, &otherStub);
    }
  }

  auto end       = std::chrono::steady_clock::now();
  double elapsed = std::chrono::duration<double, std::milli>(end - start).count();

  std::cout << "[  PERF   ] getReachableTiles for " << ENTITY_COUNT << " entities: " << elapsed << " ms" << std::endl;

  EXPECT_LT(elapsed, TARGET_MS);
}

