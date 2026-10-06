-- Core/WestAPI.lua: OnEvent routes every engine event to the right handler.
-- All game modules are spies, so only the routing itself is under test.
local T = require("harness")
local S = require("stubs")

local IDLE, MOVING, ATTACKING = 0, 1, 4
local E = S.Events

local entityStates
local transitionAccepted

T.before_each(function()
  S.reset()
  entityStates = {}
  transitionAccepted = true
  S.spy_module("Core.GameState", {
    EntityStates = { IDLE = IDLE, MOVING = MOVING, INSPECTING = 2, IN_ACTION = 3, ATTACKING = ATTACKING },
    GetEntityState = function(id) return entityStates[id] end,
    TransitionEntityState = function(id, _, new)
      if transitionAccepted then entityStates[id] = new end
    end,
    EntityHasActionPointsLeft = function() return 2 end,
    GetGameState = function() return 1 end,
  })
  S.spy_module("Core.GameLogic")
  S.spy_module("Interface.InterfaceLogic")
  S.spy_module("Core.EntityUtils")
  S.spy_module("ArtificialIntelligence.AIController")
  dofile(WEST_SCRIPTS_DIR .. "/Core/WestAPI.lua")
  Init()
end)

local function total_module_calls()
  return S.module_calls("Core.GameState") + S.module_calls("Core.GameLogic")
      + S.module_calls("Interface.InterfaceLogic") + S.module_calls("Core.EntityUtils")
      + S.module_calls("ArtificialIntelligence.AIController")
end

-- ─── Routing ───────────────────────────────────────────────

T.test("ENTITY_CREATED registers the entity", function()
  OnEvent(E.ENTITY_CREATED, { entityId = 5, playable = true, health = 90 })
  T.eq(S.last("Core.GameState.RegisterEntity"), { 5, true, 90 })
end)

T.test("ENTITY_DESTROYED removes the entity", function()
  OnEvent(E.ENTITY_DESTROYED, { entityId = 5 })
  T.eq(S.last("Core.GameState.RemoveEntity"), { 5 })
end)

T.test("ENTITY_STATE_CHANGE forwards old and new state", function()
  OnEvent(E.ENTITY_STATE_CHANGE, { entityId = 5, oldState = MOVING, newState = IDLE })
  T.eq(S.last("Core.GameState.TransitionEntityState"), { 5, MOVING, IDLE })
end)

T.test("ENTITY_RCLICK opens the entity info", function()
  OnEvent(E.ENTITY_RCLICK, { entityId = 8 })
  T.eq(S.last("Core.GameLogic.RclickEntity"), { 8 })
end)

T.test("ENTITY_ATTACK runs the attack between ATTACKING and IDLE", function()
  entityStates[3] = IDLE
  OnEvent(E.ENTITY_ATTACK, {
    attacker = 3, target = 4, bulletType = 1, range = 5, distance = 2,
    damage = 25, accuracy = 85.5, spawnX = 1.5, spawnY = 2.5,
  })
  local transitions = S.calls_of("Core.GameState.TransitionEntityState")
  T.eq(#transitions, 2)
  T.eq({ transitions[1][1], transitions[1][2], transitions[1][3] }, { 3, IDLE, ATTACKING })
  T.eq({ transitions[2][1], transitions[2][2], transitions[2][3] }, { 3, ATTACKING, IDLE })
  T.eq(S.last("Core.GameLogic.Attack"), { 3, 1, 5, 2, 25, 85.5, 4, 1.5, 2.5 })
end)

T.test("ENTITY_ATTACK is dropped when the state machine refuses ATTACKING", function()
  entityStates[3] = IDLE
  transitionAccepted = false
  OnEvent(E.ENTITY_ATTACK, { attacker = 3, target = 4, bulletType = 1, range = 5, distance = 2, damage = 25,
    accuracy = 85.5, spawnX = 1.5, spawnY = 2.5 })
  T.eq(S.count("Core.GameLogic.Attack"), 0)
end)

T.test("ENTITY_QUEUE drains the entity queue (nil payload)", function()
  OnEvent(E.ENTITY_QUEUE, nil)
  T.eq(S.count("Core.EntityUtils.DrainQueue"), 1)
end)

T.test("TILE_LCLICK moves the selected entity", function()
  entityStates[5] = IDLE
  OnEvent(E.TILE_LCLICK, { entityId = 5 })
  T.eq(S.last("Core.GameState.TransitionEntityState"), { 5, IDLE, MOVING })
end)

T.test("TILE_RCLICK only logs", function()
  OnEvent(E.TILE_RCLICK, { entityId = 5 })
  T.eq(total_module_calls(), 0)
  T.eq(S.logs_at("Info"), 1)
end)

T.test("UI_REFRESH refreshes the interfaces (nil payload)", function()
  OnEvent(E.UI_REFRESH, nil)
  T.eq(S.count("Interface.InterfaceLogic.RefreshInterfaces"), 1)
end)

T.test("UI_INTERNAL_CALL forwards the handler name and element id", function()
  OnEvent(E.UI_INTERNAL_CALL, { functionName = "endturn", elementId = 12 })
  T.eq(S.last("Core.GameLogic.Internal"), { "endturn", 12 })
end)

T.test("AI_THINK runs the AI controller for the entity", function()
  OnEvent(E.AI_THINK, { entityId = 7 })
  T.eq(S.last("ArtificialIntelligence.AIController.execute"), { 7 })
end)

T.test("LEVEL_END checks the level end condition (nil payload)", function()
  OnEvent(E.LEVEL_END, nil)
  T.eq(S.count("Core.GameState.LevelEnd"), 1)
end)

-- ─── Unknown and C++-only events ───────────────────────────

T.test("unknown event id does not error and calls nobody", function()
  T.no_error(function() OnEvent(9999, { entityId = 1 }) end)
  T.eq(total_module_calls(), 0)
  T.eq(S.logs_at("Error"), 1, "DEBUG builds report the missing handler")
end)

T.test("unknown event id is silent outside DEBUG", function()
  DEBUG = false
  T.no_error(function() OnEvent(9999, nil) end)
  T.eq(S.count("westLog"), 0)
end)

T.test("events consumed only in C++ are ignored without error", function()
  for _, name in ipairs({ "GAME_EVENT", "ATTACK_EVENT", "MOUSE_MOVE", "MOUSE_LCLICK", "MOUSE_RCLICK",
    "MOUSE_WHEEL", "KEY", "INTERFACE_UPDATE", "ACTION_FINISHED" }) do
    T.truthy(E[name] ~= nil, name .. " missing from InternalEvents.hpp")
    T.no_error(function() OnEvent(E[name], { x = 0, y = 0 }) end, name)
  end
  T.eq(total_module_calls(), 0)
end)

-- ─── Queries ───────────────────────────────────────────────

T.test("GetState, GetActionPoints and GetGameState delegate to the state machine", function()
  entityStates[4] = MOVING
  T.eq(GetState(4), MOVING)
  T.eq(GetActionPoints(4), 2)
  T.eq(GetGameState(), 1)
end)
