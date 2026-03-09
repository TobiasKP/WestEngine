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
    MoveCurPlayer(id);
  end
end

function StateChange(id, state)
  if DEBUG then
    print(playerB.getCurrentState());
    print(state);
  end
  playerB.setState(state);
  if (state == playerB.IDLE) then
    ActionFinished(id);
  end
end

function RefreshInterfaces(callee)
  local registeredUis, additionalData = table.unpack(uimanager.get());
  local height, width                 = getScreenResolution();
  for id, uis in pairs(registeredUis) do
    local result = destroyInterface(id);
    if result == false then
      print("Error deleting UI with id " .. id)
    end
    local func, stretchX, stretchY, alpha, rows, columns, hidden = table.unpack(additionalData[id])
    local x, y = func(width, height);
    uis[1].position.x = x;
    uis[1].position.y = y;
    local newId = createInterface(uis, x, y, stretchX, stretchY, alpha, rows, columns, hidden);
    uimanager.unregister(id)
    uimanager.register(newId, uis, { func, stretchX, stretchY, alpha, rows, columns, hidden });
  end
end
