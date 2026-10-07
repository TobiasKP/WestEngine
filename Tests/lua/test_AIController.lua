-- ArtificialIntelligence/AIController.lua on top of the real GameState.
local T = require("harness")
local S = require("stubs")

local IDLE, MOVING, ATTACKING = 0, 1, 4
local PLAYER_TURN = 0
local NPC, OTHER_NPC, PLAYER = 10, 11, 1

local GS, AI

local function world(tiles, enemies)
  S.returns.gatherWorldInformation = function()
    return { TilesInRange = tiles, EnemiesInRange = enemies }
  end
end

-- Spends `n` of the NPC's action points through the state machine
local function spend(id, n)
  for _ = 1, n do
    GS.TransitionEntityState(id, IDLE, MOVING)
    GS.TransitionEntityState(id, MOVING, IDLE)
  end
end

T.before_each(function()
  S.reset()
  S.spy_module("Interface.InterfaceLogic")
  GS = require("Core.GameState")
  AI = require("ArtificialIntelligence.AIController")
  GS.RegisterEntity(PLAYER, true, 100)
  GS.RegisterEntity(NPC, false, 50)
  GS.ToggleGameState() -- AI turn
  S.calls = {}
end)

T.test("with tiles in range and points to spare the NPC moves to the first tile", function()
  world({ 23, 24 }, { 7 })
  AI.execute(NPC)
  T.eq(S.last("aiMoveCommand"), { NPC, 23 })
  T.eq(S.count("aiAttackCommand"), 0)
  T.eq(GS.GetEntityState(NPC), MOVING)
  T.eq(GS.EntityHasActionPointsLeft(NPC), 2)
end)

T.test("with no enemy in range the NPC moves even on its last point", function()
  spend(NPC, 2)
  world({ 31 }, {})
  AI.execute(NPC)
  T.eq(S.last("aiMoveCommand"), { NPC, 31 })
  T.eq(GS.EntityHasActionPointsLeft(NPC), 0)
end)

T.test("on its last point with an enemy in range the NPC attacks", function()
  spend(NPC, 2)
  world({ 31 }, { 77 })
  AI.execute(NPC)
  T.eq(S.last("aiAttackCommand"), { NPC, 77 })
  T.eq(S.count("aiMoveCommand"), 0)
  T.eq(GS.GetEntityState(NPC), ATTACKING)
end)

T.test("execute first returns the NPC to IDLE after its previous action", function()
  world({ 23 }, {})
  AI.execute(NPC)
  T.eq(GS.GetEntityState(NPC), MOVING)
  AI.execute(NPC)
  T.eq(S.count("aiMoveCommand"), 2, "second move must be accepted, so the NPC was idle again")
  T.eq(GS.EntityHasActionPointsLeft(NPC), 1)
end)

T.test("out of points while another NPC still has some ends only this NPC's action", function()
  GS.RegisterEntity(OTHER_NPC, false, 50)
  spend(NPC, 3)
  AI.execute(NPC)
  T.eq(S.last("aiEndAction"), { false })
  T.eq(S.count("gatherWorldInformation"), 0)
  T.eq(S.count("dispatchEvent"), 0)
end)

T.test("when every NPC is out of points the AI turn ends", function()
  spend(NPC, 3)
  AI.execute(NPC)
  T.eq(S.last("aiEndAction"), { true })
  T.eq(GS.GetGameState(), PLAYER_TURN)
  T.eq(S.last("dispatchEvent"), { PLAYER_TURN })
end)

T.test("with no tiles and no enemies in range the NPC ends its action", function()
  world({}, {})
  AI.execute(NPC)
  T.eq(S.count("aiMoveCommand"), 0)
  T.eq(S.last("aiEndAction"), { true })
  T.eq(GS.GetGameState(), PLAYER_TURN)
  T.eq(S.last("dispatchEvent"), { PLAYER_TURN })
end)

T.test("with no tiles while another NPC still has points only this NPC's action ends", function()
  GS.RegisterEntity(OTHER_NPC, false, 50)
  world({}, {})
  AI.execute(NPC)
  T.eq(GS.EntityHasActionPointsLeft(NPC), 0)
  T.eq(S.last("aiEndAction"), { false })
  T.eq(S.count("dispatchEvent"), 0)
end)

T.test("with no tiles but an enemy in range the NPC attacks", function()
  world({}, { 77 })
  AI.execute(NPC)
  T.eq(S.last("aiAttackCommand"), { NPC, 77 })
  T.eq(S.count("aiEndAction"), 0)
  T.eq(GS.GetEntityState(NPC), ATTACKING)
end)

T.test("when the move command fails the NPC ends its action", function()
  world({ 23 }, {})
  S.returns.aiMoveCommand = function() return false end
  AI.execute(NPC)
  T.eq(S.last("aiMoveCommand"), { NPC, 23 })
  T.eq(GS.GetEntityState(NPC), IDLE)
  T.eq(S.last("aiEndAction"), { true })
  T.eq(GS.GetGameState(), PLAYER_TURN)
end)
