local StateMachine
local Interfaces
local Entitites
local Logic

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

function Entity_attack(wRange, distanceToTarget, wDmg, wAccuracy, target, spawnX, spawnY)
  Logic.Attack(wRange, distanceToTarget, wDmg, wAccuracy, target, spawnX, spawnY)
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
