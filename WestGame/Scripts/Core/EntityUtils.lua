Utils = {}

local yaml = require("Lib.yaml")
local uimanager = require("Interface.GameUIRegistry")
local builder = require("Interface.InterfaceBuilder")
local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^/]+$", "")

local playerHealthUI = {
  { type = 1, position = { x = 0, y = 0, stretchX = 0.75, stretchY = 0.75 }, color = { r = 215, g = 207, b = 196, a = 1.0 }, gridPosition = { row = 0, column = 0, count = 0 },  text = "" },
  { type = 7, position = { x = 0, y = 0, stretchX = 1.0, stretchY = 0.75 },  color = { r = 142, g = 59, b = 70, a = 1.0 },   gridPosition = { row = 0, column = 0, count = 10 }, progress = 100 }
};

local npcHealthUI = "";

local function interpreteData(data, source)
  local height, width = getScreenResolution();
  if DEBUG then
    print("found resolution: " .. width .. " : " .. height)
  end
  createEntity(data.name);

  addComponent("object", data.model);
  addComponent("shader", data.shader);

  local health = nil
  for _, component in ipairs(data.components) do
    if component.name == "health" then
      health = component
      break
    end
  end

  if data.playercontrol then
    addComponent("playercontrol");
    if health ~= nil then
      builder.Panel("playercontrol")
          :anchor("top-right", -10, 60)
          :size(10, 0.75)
          :alpha(0.5)
          :add(builder.Label(tostring(health.c))
            :color(215, 207, 196, 1.0)
            :span(string.len(tostring(health.c))))
          :add(builder.ProgressBar(100)
            :color(142, 59, 70, 1.0)
            :span(10))
          :build()
    end
  else
    --createInterface(npcHealthUI)
  end

  -- Process components
  if type(data.components) == "table" then
    for _, component in ipairs(data.components) do
      addComponent(component.name, component);
    end
  end

  buildEntity();
  if DEBUG then
    print("Creating Entity " .. data.name);
  end
end

function loadFile(name)
  local file = nil

  file = io.open(root .. name .. ".yaml")

  if file == nil then
    print("Error opening file")
    return 1
  end

  local content = file:read("*all")
  file:close()

  local data = yaml.eval(content)
  return data
end

function LoadEntity(entity)
  assert(entity ~= nil)
  local data = loadFile(entity)
  if data == nil then
    return 1
  end

  interpreteData(data, entity)
end

function world(name)
  assert(name ~= nil)
  local data = loadFile(name)
  if data == nil then
    return 1
  end
  loadWorld(data.world);
end

Utils.LoadEntity = LoadEntity
Utils.World = world;

return Utils
