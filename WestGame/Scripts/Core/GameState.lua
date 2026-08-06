StateMachine = {}

local interface = require("Interface.InterfaceLogic")
local utils = require("Utils.Utils")

local EntityStates = {
  IDLE       = 0,
  MOVING     = 1,
  INSPECTING = 2,
  IN_ACTION  = 3,
  ATTACKING  = 4,
}

local GameStates = { PLAYER_TURN = 0, AI_TURN = 1, LEVEL_ENDED = 2 }
local GameState = GameStates.PLAYER_TURN
local PlayerEntitiesState = {}
local NpcEntitiesState = {}
local PlayerEntitiesActionPoints = {}
local NpcEntitiesActionPoints = {}

function ToggleGameState()
  if GameState == GameStates.PLAYER_TURN then
    GameState = GameStates.AI_TURN
    print("GGG - Starting AI Turn")
    for i, _ in pairs(PlayerEntitiesActionPoints) do
      PlayerEntitiesActionPoints[i] = 3
      interface.SetActionPoints(i, 3)
    end
  elseif GameState == GameStates.AI_TURN then
    GameState = GameStates.PLAYER_TURN
    for i, _ in pairs(NpcEntitiesActionPoints) do
      NpcEntitiesActionPoints[i] = 3
      interface.EndTurnButton()
    end
  else
    print("GGG - Invalid Game State")
  end
  dispatchEvent(GameState)
end

function GetGameState()
  return GameState;
end

function GetEntityState(id)
  if PlayerEntitiesState[id] ~= nil then
    return PlayerEntitiesState[id]
  elseif NpcEntitiesState[id] ~= nil then
    return NpcEntitiesState[id]
  else
    print("GGG - Invalid State for Entity, no entry found for: " .. id)
  end
end

function SetPlayerEntityState(id, current, state)
  print("GGG - Setting state from: " .. current .. " to: " .. state)
  if state == EntityStates.IDLE then
    PlayerEntitiesState[id] = EntityStates.IDLE
    return
  end

  local points = PlayerEntitiesActionPoints[id]
  if points == 0 or points == nil then
    print("GGG - No more action points for entity " .. id)
    return
  end
  if state == EntityStates.MOVING then
    if current ~= EntityStates.IDLE then
      print("GGG - Already Moving")
      return
    end
    print("GGG - Moving")
    PlayerEntitiesState[id] = EntityStates.MOVING
    PlayerEntitiesActionPoints[id] = points - 1
  elseif state == EntityStates.IN_ACTION then
    if current ~= EntityStates.IDLE then
      print("GGG - Already in some engagement, not idle.")
      return
    end
    print("GGG - starting Action.")
    PlayerEntitiesActionPoints[id] = points - 1
    PlayerEntitiesState[id] = EntityStates.IN_ACTION
  elseif state == EntityStates.INSPECTING then
    print("GGG - starting Inspection")
    PlayerEntitiesActionPoints[id] = points - 1
    PlayerEntitiesState[id] = EntityStates.INSPECTING
  elseif state == EntityStates.ATTACKING then
    print("GGG - Attacking")
    PlayerEntitiesActionPoints[id] = points - 1
    PlayerEntitiesState[id] = EntityStates.ATTACKING
  end
  print("GGG - reducing action points")
  interface.SetActionPoints(id, points - 1)
end

function AIFinished()
  for i, _ in pairs(NpcEntitiesActionPoints) do
    if NpcEntitiesActionPoints[i] > 0 then
      return false;
    end
  end
  return true;
end

function EntityHasActionPointsLeft(id)
  if PlayerEntitiesActionPoints[id] then
    return PlayerEntitiesActionPoints[id]
  elseif NpcEntitiesActionPoints[id] then
    return NpcEntitiesActionPoints[id]
  else
    print("GGG - Not found " .. id);
    return -1
  end
end

function SetNpcState(id, current, state)
  if state == EntityStates.IDLE then
    NpcEntitiesState[id] = EntityStates.IDLE
    return
  end

  local points = NpcEntitiesActionPoints[id]
  if points == 0 or points == nil then
    print("GGG - No more action points for entity " .. id)
    return
  end

  if state == EntityStates.MOVING then
    if current ~= EntityStates.IDLE then
      print("GGG - Already Moving")
      return
    end
    NpcEntitiesState[id] = EntityStates.MOVING
    NpcEntitiesActionPoints[id] = points - 1
  elseif state == EntityStates.IN_ACTION then
    if current ~= EntityStates.IDLE then
      print("GGG - Already in Action")
      return
    end
    NpcEntitiesState[id] = EntityStates.IN_ACTION
    NpcEntitiesActionPoints[id] = points - 1
  elseif state == EntityStates.INSPECTING then
    NpcEntitiesState[id] = EntityStates.INSPECTING
    NpcEntitiesActionPoints[id] = points - 1
  elseif state == EntityStates.ATTACKING then
    NpcEntitiesState[id] = EntityStates.ATTACKING
    NpcEntitiesActionPoints[id] = points - 1
  end
end

function TransitionEntityState(id, current, newState)
  if PlayerEntitiesState[id] ~= nil and GameState == GameStates.PLAYER_TURN then
    SetPlayerEntityState(id, current, newState)
  elseif NpcEntitiesState[id] ~= nil and GameState == GameStates.AI_TURN then
    SetNpcState(id, current, newState)
  else
    print("GGG - Invalid State for Entity, no entry found for: " .. id)
  end
end

function RegisterEntity(id, playable, health)
  print("GGG - Registering entity " .. id)
  if playable then
    PlayerEntitiesState[id] = EntityStates.IDLE
    PlayerEntitiesActionPoints[id] = 3;
    interface.AddActionPointsUI(id)
    interface.AddPlayerHealth(id, health);
  else
    NpcEntitiesState[id] = EntityStates.IDLE
    NpcEntitiesActionPoints[id] = 3;
  end
end

function RemoveEntity(id)
  print("GGG - Removing entity " .. id)
  PlayerEntitiesState[id] = nil
  PlayerEntitiesActionPoints[id] = nil
  NpcEntitiesState[id] = nil
  NpcEntitiesActionPoints[id] = nil
end

function LevelEnd()
  if GameState == GameStates.LEVEL_ENDED then
    return
  end
  if utils.TableLength(PlayerEntitiesState) == 0 then
    interface.LevelEndScreen(false)
    GameState = GameStates.LEVEL_ENDED
  elseif utils.TableLength(NpcEntitiesState) == 0 then
    interface.LevelEndScreen(true)
    GameState = GameStates.LEVEL_ENDED
  end
end

StateMachine.EntityStates = EntityStates
StateMachine.GetGameState = GetGameState
StateMachine.ToggleGameState = ToggleGameState
StateMachine.GetEntityState = GetEntityState
StateMachine.TransitionEntityState = TransitionEntityState
StateMachine.RegisterEntity = RegisterEntity
StateMachine.RemoveEntity = RemoveEntity
StateMachine.EntityHasActionPointsLeft = EntityHasActionPointsLeft
StateMachine.AIFinished = AIFinished
StateMachine.LevelEnd = LevelEnd

return StateMachine
