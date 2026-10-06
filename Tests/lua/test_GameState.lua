-- Core/GameState.lua: turn handling, entity states and action points.
local T = require("harness")
local S = require("stubs")

local IDLE, MOVING, INSPECTING, IN_ACTION, ATTACKING = 0, 1, 2, 3, 4
local PLAYER_TURN, AI_TURN, LEVEL_ENDED = 0, 1, 2

local GS

T.before_each(function()
  S.reset()
  S.spy_module("Interface.InterfaceLogic")
  GS = require("Core.GameState")
end)

-- ─── Registration ──────────────────────────────────────────

T.test("exported EntityStates match the values C++ uses (LuaFacade::LuaStates)", function()
  T.eq(GS.EntityStates, { IDLE = IDLE, MOVING = MOVING, INSPECTING = INSPECTING, IN_ACTION = IN_ACTION,
    ATTACKING = ATTACKING })
end)

T.test("game starts in the player turn", function()
  T.eq(GS.GetGameState(), PLAYER_TURN)
end)

T.test("RegisterEntity player starts IDLE with 3 action points and gets its UI", function()
  GS.RegisterEntity(1, true, 90)
  T.eq(GS.GetEntityState(1), IDLE)
  T.eq(GS.EntityHasActionPointsLeft(1), 3)
  T.eq(S.last("Interface.InterfaceLogic.AddActionPointsUI"), { 1 })
  T.eq(S.last("Interface.InterfaceLogic.AddPlayerHealth"), { 1, 90 })
end)

T.test("RegisterEntity NPC starts IDLE with 3 action points and no player UI", function()
  GS.RegisterEntity(2, false, 50)
  T.eq(GS.GetEntityState(2), IDLE)
  T.eq(GS.EntityHasActionPointsLeft(2), 3)
  T.eq(S.module_calls("Interface.InterfaceLogic"), 0)
end)

T.test("RemoveEntity forgets state and action points", function()
  GS.RegisterEntity(1, true, 90)
  GS.RemoveEntity(1)
  T.eq(GS.GetEntityState(1), nil)
  T.eq(GS.EntityHasActionPointsLeft(1), -1)
end)

-- ─── Transitions ───────────────────────────────────────────

T.test("IDLE -> MOVING costs one action point and updates the UI", function()
  GS.RegisterEntity(1, true, 90)
  GS.TransitionEntityState(1, IDLE, MOVING)
  T.eq(GS.GetEntityState(1), MOVING)
  T.eq(GS.EntityHasActionPointsLeft(1), 2)
  T.eq(S.last("Interface.InterfaceLogic.SetActionPoints"), { 1, 2 })
end)

T.test("MOVING -> MOVING is rejected and costs nothing", function()
  GS.RegisterEntity(1, true, 90)
  GS.TransitionEntityState(1, IDLE, MOVING)
  GS.TransitionEntityState(1, MOVING, MOVING)
  T.eq(GS.GetEntityState(1), MOVING)
  T.eq(GS.EntityHasActionPointsLeft(1), 2)
end)

T.test("transition back to IDLE is always allowed and free", function()
  GS.RegisterEntity(1, true, 90)
  GS.TransitionEntityState(1, IDLE, MOVING)
  GS.TransitionEntityState(1, MOVING, IDLE)
  T.eq(GS.GetEntityState(1), IDLE)
  T.eq(GS.EntityHasActionPointsLeft(1), 2)
end)

T.test("IN_ACTION from a non-idle state is rejected", function()
  GS.RegisterEntity(1, true, 90)
  GS.TransitionEntityState(1, IDLE, MOVING)
  GS.TransitionEntityState(1, MOVING, IN_ACTION)
  T.eq(GS.GetEntityState(1), MOVING)
  T.eq(GS.EntityHasActionPointsLeft(1), 2)
end)

T.test("no transition is accepted once the action points are spent", function()
  GS.RegisterEntity(1, true, 90)
  for _ = 1, 3 do
    GS.TransitionEntityState(1, IDLE, MOVING)
    GS.TransitionEntityState(1, MOVING, IDLE)
  end
  T.eq(GS.EntityHasActionPointsLeft(1), 0)
  GS.TransitionEntityState(1, IDLE, MOVING)
  T.eq(GS.GetEntityState(1), IDLE)
  GS.TransitionEntityState(1, IDLE, ATTACKING)
  T.eq(GS.GetEntityState(1), IDLE)
  T.eq(GS.EntityHasActionPointsLeft(1), 0)
end)

T.test("NPC transition during the player turn is rejected", function()
  GS.RegisterEntity(2, false, 50)
  GS.TransitionEntityState(2, IDLE, MOVING)
  T.eq(GS.GetEntityState(2), IDLE)
  T.eq(GS.EntityHasActionPointsLeft(2), 3)
end)

T.test("player transition during the AI turn is rejected", function()
  GS.RegisterEntity(1, true, 90)
  GS.RegisterEntity(2, false, 50)
  GS.ToggleGameState()
  GS.TransitionEntityState(1, IDLE, MOVING)
  T.eq(GS.GetEntityState(1), IDLE)
  T.eq(GS.EntityHasActionPointsLeft(1), 3)
end)

T.test("NPC transition during the AI turn costs one action point", function()
  GS.RegisterEntity(2, false, 50)
  GS.ToggleGameState()
  GS.TransitionEntityState(2, IDLE, ATTACKING)
  T.eq(GS.GetEntityState(2), ATTACKING)
  T.eq(GS.EntityHasActionPointsLeft(2), 2)
end)

-- ─── Turn toggling ─────────────────────────────────────────

T.test("ToggleGameState player -> AI resets player points and notifies C++", function()
  GS.RegisterEntity(1, true, 90)
  GS.TransitionEntityState(1, IDLE, MOVING)
  T.eq(GS.EntityHasActionPointsLeft(1), 2)

  GS.ToggleGameState()

  T.eq(GS.GetGameState(), AI_TURN)
  T.eq(GS.EntityHasActionPointsLeft(1), 3)
  T.eq(S.last("Interface.InterfaceLogic.SetActionPoints"), { 1, 3 })
  T.eq(S.count("dispatchEvent"), 1)
  T.eq(S.last("dispatchEvent"), { AI_TURN })
end)

T.test("ToggleGameState AI -> player resets NPC points and notifies C++", function()
  GS.RegisterEntity(2, false, 50)
  GS.ToggleGameState()
  GS.TransitionEntityState(2, IDLE, MOVING)
  T.eq(GS.EntityHasActionPointsLeft(2), 2)

  GS.ToggleGameState()

  T.eq(GS.GetGameState(), PLAYER_TURN)
  T.eq(GS.EntityHasActionPointsLeft(2), 3)
  T.eq(S.count("dispatchEvent"), 2)
  T.eq(S.last("dispatchEvent"), { PLAYER_TURN })
end)

-- BUG: WestGame/Scripts/Core/GameState.lua:33 interface.EndTurnButton() is called inside the loop over NPCs, so the player gets one end-turn button per NPC (and none at all when no NPC is registered).
T.skip("returning to the player turn shows exactly one end-turn button",
  "BUG: WestGame/Scripts/Core/GameState.lua:33 EndTurnButton is created once per NPC instead of once per turn",
  function()
    GS.RegisterEntity(2, false, 50)
    GS.RegisterEntity(3, false, 50)
    GS.ToggleGameState()
    GS.ToggleGameState()
    T.eq(S.count("Interface.InterfaceLogic.EndTurnButton"), 1)
  end)

T.test("AIFinished is true only when every NPC spent its points", function()
  GS.RegisterEntity(2, false, 50)
  GS.RegisterEntity(3, false, 50)
  GS.ToggleGameState()
  for _ = 1, 3 do
    GS.TransitionEntityState(2, IDLE, MOVING)
    GS.TransitionEntityState(2, MOVING, IDLE)
  end
  T.falsy(GS.AIFinished())
  for _ = 1, 3 do
    GS.TransitionEntityState(3, IDLE, MOVING)
    GS.TransitionEntityState(3, MOVING, IDLE)
  end
  T.truthy(GS.AIFinished())
end)

-- ─── Level end ─────────────────────────────────────────────

T.test("LevelEnd with players left and no NPCs is a win", function()
  GS.RegisterEntity(1, true, 90)
  GS.LevelEnd()
  T.eq(S.last("Interface.InterfaceLogic.LevelEndScreen"), { true })
  T.eq(GS.GetGameState(), LEVEL_ENDED)
end)

T.test("LevelEnd with no players left is a loss", function()
  GS.RegisterEntity(2, false, 50)
  GS.LevelEnd()
  T.eq(S.last("Interface.InterfaceLogic.LevelEndScreen"), { false })
  T.eq(GS.GetGameState(), LEVEL_ENDED)
end)

T.test("LevelEnd after the last NPC was removed is a win", function()
  GS.RegisterEntity(1, true, 90)
  GS.RegisterEntity(2, false, 50)
  GS.RemoveEntity(2)
  GS.LevelEnd()
  T.eq(S.last("Interface.InterfaceLogic.LevelEndScreen"), { true })
end)

T.test("LevelEnd while both sides are alive changes nothing", function()
  GS.RegisterEntity(1, true, 90)
  GS.RegisterEntity(2, false, 50)
  GS.LevelEnd()
  T.eq(S.count("Interface.InterfaceLogic.LevelEndScreen"), 0)
  T.eq(GS.GetGameState(), PLAYER_TURN)
end)

T.test("LevelEnd only shows the end screen once", function()
  GS.RegisterEntity(1, true, 90)
  GS.LevelEnd()
  GS.LevelEnd()
  T.eq(S.count("Interface.InterfaceLogic.LevelEndScreen"), 1)
end)
