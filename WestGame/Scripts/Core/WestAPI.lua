local StateMachine
local Interfaces
local Logic
local Entities
local AIController

function Init()
  StateMachine = require("Core.GameState")
  Logic = require("Core.GameLogic")
  Interfaces = require("Interface.InterfaceLogic")
  Entities = require("Core.EntityUtils")
  AIController = require("ArtificialIntelligence.AIController")
end

-- Every notification coming from C++ arrives in OnEvent and is routed by the
-- event identifier. Adding one means adding an entry to this table, the field
-- names of the payload come from EventPayload.cpp.
local Handlers = {
  [Events.ENTITY_CREATED] = function(payload)
    StateMachine.RegisterEntity(payload.entityId, payload.playable, payload.health)
  end,

  [Events.ENTITY_DESTROYED] = function(payload)
    StateMachine.RemoveEntity(payload.entityId)
  end,

  [Events.ENTITY_STATE_CHANGE] = function(payload)
    StateMachine.TransitionEntityState(payload.entityId, payload.oldState, payload.newState)
  end,

  [Events.ENTITY_RCLICK] = function(payload)
    Logic.RclickEntity(payload.entityId)
  end,

  [Events.ENTITY_ATTACK] = function(payload)
    local attacker = payload.attacker
    StateMachine.TransitionEntityState(attacker, StateMachine.GetEntityState(attacker),
      StateMachine.EntityStates.ATTACKING)
    if StateMachine.GetEntityState(attacker) ~= StateMachine.EntityStates.ATTACKING then
      return
    end
    Logic.Attack(attacker, payload.bulletType, payload.range, payload.distance, payload.damage, payload.accuracy,
      payload.target, payload.spawnX, payload.spawnY)
    StateMachine.TransitionEntityState(attacker, StateMachine.EntityStates.ATTACKING, StateMachine.EntityStates.IDLE)
  end,

  [Events.ENTITY_QUEUE] = function()
    Entities.DrainQueue()
  end,

  [Events.TILE_LCLICK] = function(payload)
    StateMachine.TransitionEntityState(payload.entityId, StateMachine.GetEntityState(payload.entityId),
      StateMachine.EntityStates.MOVING)
  end,

  [Events.TILE_RCLICK] = function(payload)
    print(payload.entityId)
  end,

  [Events.UI_REFRESH] = function()
    Interfaces.RefreshInterfaces()
  end,

  [Events.UI_INTERNAL_CALL] = function(payload)
    Logic.Internal(payload.functionName, payload.elementId)
  end,

  [Events.AI_THINK] = function(payload)
    AIController.execute(payload.entityId)
  end,

  [Events.LEVEL_END] = function()
    StateMachine.LevelEnd()
  end,
}

function OnEvent(event, payload)
  local handler = Handlers[event]
  if handler == nil then
    if DEBUG then
      print("GGG - No handler registered for event: " .. tostring(event))
    end
    return
  end
  handler(payload)
end

-- Queries, C++ asks for state that lives here and needs an answer right away
function GetGameState()
  return StateMachine.GetGameState()
end

function GetState(id)
  return StateMachine.GetEntityState(id)
end

function GetActionPoints(id)
  return StateMachine.EntityHasActionPointsLeft(id)
end
