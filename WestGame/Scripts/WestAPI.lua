local playerB
local uimanager

function Init()
  playerB = require("Player/PlayerBehaviour")
  uimanager = require("GameUIRegistry")
end

function Worldpos_lclick(id, x, y, z)
  if DEBUG then
    print("clicked pos: " .. x .. y .. z);
  end
  local state = playerB.getCurrentState();
  if state == playerB.IDLE then
    playerB.setState(playerB.MOVING);
  end
end

function Worldpos_rclick(id, x, y, z)
end

function Entity_lclick(id, entity)
end

function Entity_rclick(id, entity)
  local health = getHealth(entity);
  print(health)
  --TODO get Health from component and display in UI
end

function GetState(id)
  return playerB.getCurrentState();
end

function StateChange(id, state)
  if DEBUG then
    print(playerB.getCurrentState());
    print(state);
  end
  playerB.setState(state);
  if state == playerB.MOVING_FINISHED then
    playerB.setState(playerB.IDLE)
  end
end

function DestroyInterface(callee, id)
  local result = destroyInterface(id)
  if result == false then
    print("Error deleting UI with id " .. id)
    return
  end
  uimanager.unregister(id)
end

function RefreshInterfaces(callee)
  local registeredUis, additionalData = table.unpack(uimanager.get());
  local height, width                 = getScreenResolution();
  local tmpUnregister                 = {};
  local tmpRegister                   = {};
  for id, uis in pairs(registeredUis) do
    local result = destroyInterface(id);
    if result == false then
      print("Error deleting UI with id " .. id)
    end
    local func, stretchX, stretchY, alpha, rows, columns, hidden = table.unpack(additionalData[id])
    local parent = additionalData[id].parent
    local x, y = func(width, height);
    for _, value in pairs(uis) do
      if type(value) == "table" and value.position then
        value.position.x = x
        value.position.y = y
      end
    end
    local newId = createInterface(uis, x, y, stretchX, stretchY, alpha, rows, columns, hidden);
    table.insert(tmpUnregister, id);
    table.insert(tmpRegister, { newId, uis, { func, stretchX, stretchY, alpha, rows, columns, hidden, parent = parent } })
  end
  for _, id in ipairs(tmpUnregister) do
    uimanager.unregister(id)
  end
  for _, entry in ipairs(tmpRegister) do
    uimanager.register(table.unpack(entry))
  end
end
