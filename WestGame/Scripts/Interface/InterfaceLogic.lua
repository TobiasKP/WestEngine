local InterfaceLogic = {}

local uimanager = require("Interface.GameUIRegistry")

function DestroyInterface(id)
  local result = destroyInterface(id)
  if result == false then
    print("Error deleting UI with id " .. id)
    return
  end
  uimanager.unregister(id)
end

function RefreshInterfaces()
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

InterfaceLogic.DestroyInterface = DestroyInterface
InterfaceLogic.RefreshInterfaces = RefreshInterfaces

return InterfaceLogic
