local InterfaceLogic = {}

local uimanager = require("Interface.GameUIRegistry")
local builder = require("Interface.InterfaceBuilder")

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
    table.insert(tmpRegister, { newId, uis,
      { func, stretchX, stretchY, alpha, rows, columns, hidden, parent = parent }
    })
  end
  for _, id in ipairs(tmpUnregister) do
    uimanager.unregister(id)
  end
  for _, entry in ipairs(tmpRegister) do
    uimanager.register(table.unpack(entry))
  end
end

function ConstructInfoPanel(id)
  local health = getHealth(id);
  local screenX, screenY = getPosition(id);
  builder.Panel("npcinfo")
      :anchor("none", screenX, screenY)
      :size(7, 7)
      :alpha(0.8)
      :add(builder.Button("X")
        :handler("closeinterface")
        :color(215, 207, 196, 1.0)
        :span(1)
        :grid(6, 6))
      :add(builder.Label("HP")
        :span(2)
        :grid(5, 0)
        :color(215, 207, 196, 1.0))
      :add(builder.ProgressBar()
        :color(142, 59, 70, 1.0)
        :span(4)
        :progress(health)
        :attachEntity(id)
        :grid(5, 2)
        :flag(0x01))
      :build()
end

function SetActionPoints(id, points)
  local uiId = uimanager.getByParent("actionpoints_" .. id);
  if not uiId or not uiId[1] then
    return
  end
  local res = uimanager.getElement(uiId[1], "actionpoints")
  if res == -1 or res == nil then
    return
  end
  updateInterfaceValue(res, 0x10, tostring(math.ceil((points / 3) * 100)))
end

function AddActionPointsUI(id)
  builder.Panel("actionpoints_" .. id)
      :anchor("bottom-left", 15, 45)
      :size(3, 1)
      :alpha(0.5)
      :add(builder.ProgressBar(100)
        :color(142, 59, 70, 1.0)
        :span(3)
        :tag("actionpoints"))
      :build()
end

function EndTurnButton()
  builder.Panel("endturn")
      :anchor("bottom-middle", 0, 80)
      :size(1, 1)
      :alpha(1.0)
      :add(builder.Button("E")
        :handler("endturn")
        :color(215, 207, 196, 1.0)
        :span(1)
        :grid(0, 0))
      :build()
end

InterfaceLogic.DestroyInterface = DestroyInterface
InterfaceLogic.RefreshInterfaces = RefreshInterfaces
InterfaceLogic.SetActionPoints = SetActionPoints
InterfaceLogic.AddActionPointsUI = AddActionPointsUI
InterfaceLogic.ConstructInfoPanel = ConstructInfoPanel
InterfaceLogic.EndTurnButton = EndTurnButton

return InterfaceLogic
