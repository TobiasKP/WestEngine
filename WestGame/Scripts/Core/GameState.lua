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
    westLog(LogLevel.Info, "Player turn ended, starting AI turn")
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
    westLog(LogLevel.Error, "Cannot toggle game state, invalid game state: " .. tostring(GameState))
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
    westLog(LogLevel.Error, "GetEntityState: no state entry found for entity " .. tostring(id))
  end
end

function SetPlayerEntityState(id, current, state)
  westLog(LogLevel.Info,
    "Player entity " .. tostring(id) .. " state transition from " .. tostring(current) .. " to " .. tostring(state))
  if state == EntityStates.IDLE then
    PlayerEntitiesState[id] = EntityStates.IDLE
    return
  end

  local points = PlayerEntitiesActionPoints[id]
  if points == 0 or points == nil then
    westLog(LogLevel.Info,
      "Player entity " .. tostring(id) .. " has no action points left, ignoring transition to state " ..
      tostring(state))
    return
  end
  if state == EntityStates.MOVING then
    if current ~= EntityStates.IDLE then
      westLog(LogLevel.Info,
        "Player entity " .. tostring(id) .. " cannot start moving, not idle (current state " .. tostring(current) ..
        ")")
      return
    end
    westLog(LogLevel.Info, "Player entity " .. tostring(id) .. " starts moving")
    PlayerEntitiesState[id] = EntityStates.MOVING
    PlayerEntitiesActionPoints[id] = points - 1
  elseif state == EntityStates.IN_ACTION then
    if current ~= EntityStates.IDLE then
      westLog(LogLevel.Info,
        "Player entity " .. tostring(id) .. " cannot start action, not idle (current state " .. tostring(current) ..
        ")")
      return
    end
    westLog(LogLevel.Info, "Player entity " .. tostring(id) .. " starts action")
    PlayerEntitiesActionPoints[id] = points - 1
    PlayerEntitiesState[id] = EntityStates.IN_ACTION
  elseif state == EntityStates.INSPECTING then
    westLog(LogLevel.Info, "Player entity " .. tostring(id) .. " starts inspection")
    PlayerEntitiesActionPoints[id] = points - 1
    PlayerEntitiesState[id] = EntityStates.INSPECTING
  elseif state == EntityStates.ATTACKING then
    westLog(LogLevel.Info, "Player entity " .. tostring(id) .. " starts attacking")
    PlayerEntitiesActionPoints[id] = points - 1
    PlayerEntitiesState[id] = EntityStates.ATTACKING
  end
  westLog(LogLevel.Info, "Player entity " .. tostring(id) .. " action points reduced to " .. tostring(points - 1))
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
    westLog(LogLevel.Error, "EntityHasActionPointsLeft: entity " .. tostring(id) .. " not found")
    return -1
  end
end

function SetNpcActionPoints(id, points)
  if NpcEntitiesActionPoints[id] then
    NpcEntitiesActionPoints[id] = points
  end
end

function SetNpcState(id, current, state)
  if state == EntityStates.IDLE then
    NpcEntitiesState[id] = EntityStates.IDLE
    return
  end

  local points = NpcEntitiesActionPoints[id]
  if points == 0 or points == nil then
    westLog(LogLevel.Info,
      "NPC entity " .. tostring(id) .. " has no action points left, ignoring transition to state " .. tostring(state))
    return
  end

  if state == EntityStates.MOVING then
    if current ~= EntityStates.IDLE then
      westLog(LogLevel.Info,
        "NPC entity " .. tostring(id) .. " cannot start moving, not idle (current state " .. tostring(current) .. ")")
      return
    end
    NpcEntitiesState[id] = EntityStates.MOVING
    NpcEntitiesActionPoints[id] = points - 1
  elseif state == EntityStates.IN_ACTION then
    if current ~= EntityStates.IDLE then
      westLog(LogLevel.Info,
        "NPC entity " .. tostring(id) .. " cannot start action, not idle (current state " .. tostring(current) .. ")")
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
    westLog(LogLevel.Error,
      "TransitionEntityState: no state entry found for entity " .. tostring(id) .. " in game state " ..
      tostring(GameState))
  end
end

function RegisterEntity(id, playable, health)
  westLog(LogLevel.Info, "Registering entity " .. tostring(id) .. (playable and " (player)" or " (NPC)"))
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
  westLog(LogLevel.Info, "Removing entity " .. tostring(id))
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
StateMachine.SetNpcActionPoints = SetNpcActionPoints
StateMachine.AIFinished = AIFinished
StateMachine.LevelEnd = LevelEnd

return StateMachine
