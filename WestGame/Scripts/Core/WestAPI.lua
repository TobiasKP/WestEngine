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

function GetGameState()
  return StateMachine.GetGameState()
end

function GetState(id)
  return StateMachine.GetEntityState(id)
end

function GetActionPoints(id)
  return StateMachine.EntityHasActionPointsLeft(id)
end

function EntityStateChange(id, oldState, newState)
  StateMachine.TransitionEntityState(id, oldState, newState)
end

function Worldpos_lclick(id)
  StateMachine.TransitionEntityState(id, StateMachine.GetEntityState(id), StateMachine.EntityStates.MOVING)
end

function Worldpos_rclick(targetId)
  print(targetId)
end

function RegisterEntity(id, playable, health)
  StateMachine.RegisterEntity(id, playable, health)
end

function RemoveEntity(id)
  StateMachine.RemoveEntity(id)
end

function Entity_attack(attackerId, bulletType, wRange, distanceToTarget, wDmg, wAccuracy, target, spawnX, spawnY)
  StateMachine.TransitionEntityState(attackerId, StateMachine.GetEntityState(attackerId),
    StateMachine.EntityStates.ATTACKING)
  if GetState(attackerId) == StateMachine.EntityStates.ATTACKING then
    Logic.Attack(attackerId, bulletType, wRange, distanceToTarget, wDmg, wAccuracy, target, spawnX, spawnY)
    EntityStateChange(attackerId, StateMachine.EntityStates.ATTACKING, StateMachine.EntityStates.IDLE)
  end
end

function Entity_rclick(targetId)
  Logic.RclickEntity(targetId)
end

function DestroyInterface(id)
  Interfaces.DestroyInterface(id)
end

function RefreshInterfaces()
  Interfaces.RefreshInterfaces()
end

function EntityQueue()
  Entities.DrainQueue()
end

function InterfaceInternalFunctionCall(functionToCall, callingButtonId)
  Logic.Internal(functionToCall, callingButtonId)
end

function AIThink(entityId)
  AIController.execute(entityId)
end

function LevelEnd()
  return StateMachine.LevelEnd()
end
