# AISystem — update() design

Design note for the first AI system. Queries are synchronous C++ returns; brains are
stateless Lua functions; C++ owns sequencing and turn-end detection.

## Responsibilities

`AISystem` is a `ISystem` in `SystemManager::_systems`. It owns:

1. The Lua query bindings (`gatherInformation`) and action bindings (`aiMoveTo`, `aiAttack`),
   registered in its constructor the way `EntityBuilder.cpp:12-14` does. It holds `_reg` and
   `_world`, which is why the bindings live here.
2. Turn-state latching via `GAME_EVENT` (same subscription pattern as `PlayerControl.cpp:36`).
3. Unit sequencing — one NPC at a time, advance when it is out of action points.
4. Turn-end detection.
5. Later: once-per-turn batched precompute (threat map, LOS cache) that `gatherInformation`
   reads cheaply.

## update()

```
update():
  pollEvents()                      -- latches turn state from GAME_EVENT
  if not aiTurn: return

  npc = currentUnit()
  if npc == none:
    tryEndTurn()
    return

  if busy(npc): return              -- mid-move or bullet resolving; wait

  if actionPoints(npc) > 0:
    AIDecide(npc)                   -- Lua: query, choose, issue ONE action, return
  else:
    advanceToNextUnit()
```

`busy(npc)` is `Movement.destination.has_value()` for that unit, or any live `Projectile`
it spawned.

### Brain contract

`AIDecide(npcId)` is a plain synchronous Lua call. It calls `gatherInformation(npcId)`,
picks **one** action, issues it via `aiMoveTo` / `aiAttack`, and returns. No coroutine, no
suspended state, no request/response correlation.

Brains are stateless because their state — action points, position, who is alive — already
lives in the world. This is also the shape behaviour trees and utility scoring want: both
re-evaluate from scratch at each decision point. The dumb-random brain and a future
tile-scoring brain share this signature.

Reach for a coroutine only when a brain needs to hold intent *across* decisions ("executing
a 3-step flank, do not re-decide"). Not needed for the first AI.

## Turn end — two facts, two owners

Lua knowing it is done issuing intents is **not** the same as the turn being over. When the
last brain returns, bullets are still in flight and units are still sliding. Toggling then
makes `ToggleGameState` refill player action points and wakes `PlayerControl`'s reachable-tile
computation while an enemy projectile is still traveling. That is a correctness bug.

- **Lua signals** "no more intents" — all NPCs out of AP, or all brains declined to act.
- **C++ decides** nothing is still resolving: no NPC has `Movement.destination`, and
  `getComponentArray<Projectile>()->getSize() == 0`.
- The turn ends on the **AND** of both. C++ owns the second half because the authoritative
  "is anything still animating" state is in the components, not in Lua.

## Gotchas

**Projectile reap lags one frame.** `ProjectileSystem` calls `e->destroy()`, but the entity is
not removed until `SceneManager::update()` — and `SceneManager` is `_manager[5]` while
`SystemManager` is `_manager[6]`, so reaping happens the *next* frame. The projectile count can
read non-zero for one frame after the last bullet resolved. Requiring zero errs toward waiting
a frame too long, which is the safe direction.

**Watchdog.** If the AI turn is active with nothing resolving and no decisions being issued for
some threshold, log a failure and force the turn back. The end-turn button is destroyed during
the AI turn (dd8c7a0), so a stuck brain otherwise leaves the player with no input at all.

**System order matters.** `_systems` is `std::array<ISystem*, 5>` with a hardcoded init list at
`SystemManager.cpp:33` — bump to 6. Placing `AISystem` *before* `MovementSystem` means a
destination issued by a brain is consumed the same frame instead of the next.

**If you switch to waypoints instead**, letting Lua decide for every NPC in one burst,
responsibilities 3 and 4 shrink to just turn-end detection and this stops deserving to be a
System — a service class owned by `SceneManager`, like `EntityBuilder`, would fit better.

## Prerequisites

- `Movement.destination` claims its tile in the world grid at issue time, not per frame
  (drop the `updateEntityIdToIdx` call at `MovementSystem.cpp:81`).
- `World::getReachableTiles` filters tiles occupied by another entity — it currently has no
  occupancy check at all (`World.cpp:120`).
- `SetNpcState` (`GameState.lua:101`) needs an `ATTACKING` branch, or `Entity_attack`'s state
  guard fails and NPCs never fire. It also never decrements NPC action points.
- NPC action points never refill: `ToggleGameState` uses `ipairs` over an entity-id-keyed table
  (`GameState.lua:29`).
- `EnemyCube.yaml` has no `equipment` component, so `ProjectileSystem` logs "no active
  weaponary" and no shot happens.
- `ProjectileSystem::travel` dereferences `getComponent<Position>(p->destination)` with no null
  check (`ProjectileSystem.cpp:50`). Two bullets toward one target crashes on the second once
  the first kills it.
