StateMachine = {}

local EntityStates = {
  IDLE       = 0,
  MOVING     = 1,
  INSPECTING = 3,
  IN_ACTION  = 5,
}

local GameStates = { PLAYER_TURN = 0, AI_TURN = 1 }

local GameState = GameStates.PLAYER_TURN
local PlayerEntitiesState = {}
local NpcEntitiesState = {}

function SetGameState(state)
  if state == GameStates.PLAYER_TURN then
    GameState = GameStates.PLAYER_TURN
  elseif state == GameStates.AI_TURN then
    GameState = GameStates.AI_TURN
  else
    print("GGG - Invalid Game State")
  end
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
  if state == EntityStates.IDLE then
    PlayerEntitiesState[id] = EntityStates.IDLE
  elseif state == EntityStates.MOVING then
    if current ~= EntityStates.IDLE then
      print("GGG - Already Moving")
      return
    end
    PlayerEntitiesState[id] = EntityStates.MOVING
  elseif state == EntityStates.IN_ACTION then
    if current ~= EntityStates.IDLE then
      print("GGG - Already in Action")
      return
    end
    PlayerEntitiesState[id] = EntityStates.IN_ACTION
  elseif state == EntityStates.INSPECTING then
    PlayerEntitiesState[id] = EntityStates.INSPECTING
  end
end

function SetNpcState(id, current, state)
  if state == EntityStates.IDLE then
    NpcEntitiesState[id] = EntityStates.IDLE
  elseif state == EntityStates.MOVING then
    if current ~= EntityStates.IDLE then
      print("GGG - Already Moving")
      return
    end
    NpcEntitiesState[id] = EntityStates.MOVING
  elseif state == EntityStates.IN_ACTION then
    if current ~= EntityStates.IDLE then
      print("GGG - Already in Action")
      return
    end
    NpcEntitiesState[id] = EntityStates.IN_ACTION
  elseif state == EntityStates.INSPECTING then
    NpcEntitiesState[id] = EntityStates.INSPECTING
  end
end

function TransitionEntityState(id, current, newState)
  if PlayerEntitiesState[id] ~= nil then
    SetPlayerEntityState(id, current, newState)
  elseif NpcEntitiesState[id] ~= nil then
    SetNpcState(id, current, newState)
  else
    print("GGG - Invalid State for Entity, no entry found for: " .. id)
  end
end

function RegisterEntity(id, playable)
  if playable then
    PlayerEntitiesState[id] = EntityStates.IDLE
  else
    NpcEntitiesState[id] = EntityStates.IDLE
  end
end

StateMachine.GetGameState = GetGameState
StateMachine.SetGameState = SetGameState
StateMachine.GetEntityState = GetEntityState
StateMachine.TransitionEntityState = TransitionEntityState
StateMachine.RegisterEntity = RegisterEntity
StateMachine.EntityStates = EntityStates

return StateMachine
