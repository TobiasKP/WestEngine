local StateMachine
local Interfaces

function Init()
  StateMachine = require("Core.GameState")
  Interfaces = require("Interface.InterfaceLogic")
end

function GetState(id)
  return StateMachine.GetEntityState(id);
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

function Entity_lclick(targetId)
  print(targetId)
end

function Entity_rclick(id)
  --TODO get Health from component and display in UI
  local health = getHealth(id);
  print(health)
end

function DestroyInterface(id)
  Interfaces.DestroyInterface(id)
end

function RefreshInterfaces()
  Interfaces.RefreshInterfaces()
end
