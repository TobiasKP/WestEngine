local StateMachine
local Interfaces
local Logic
local Entities

function Init()
  StateMachine = require("Core.GameState")
  Logic = require("Core.GameLogic")
  Interfaces = require("Interface.InterfaceLogic")
  Entities = require("Core.EntityUtils")
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

function RegisterEntity(id, playable)
  StateMachine.RegisterEntity(id, playable)
end

function Entity_attack(attackerId, bulletType, wRange, distanceToTarget, wDmg, wAccuracy, target, spawnX, spawnY)
  StateMachine.TransitionEntityState(attackerId, StateMachine.GetEntityState(attackerId),
    StateMachine.EntityStates.ATTACKING)
  if GetState(attackerId) == StateMachine.EntityStates.ATTACKING then
    Logic.Attack(bulletType, wRange, distanceToTarget, wDmg, wAccuracy, target, spawnX, spawnY)
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
