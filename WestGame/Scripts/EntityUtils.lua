Utils = {}

local yaml = require("yaml")
local uimanager = require("GameUIRegistry")
local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^/]+$", "")

local playerHealthUI = {
  { type = 1, position = { x = 0, y = 0, stretchX = 0.75, stretchY = 0.75 }, color = { r = 215, g = 207, b = 196, a = 1.0 }, gridPosition = { row = 0, column = 0, count = 2 },  text = "" },
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
      local text = tostring(health.c)
      playerHealthUI[1].text = text;
      playerHealthUI[1].gridPosition.count = string.len(text);
      local x = width - 10 * 40 - 10;
      local y = 60;
      playerHealthUI[1].position.x = x;
      playerHealthUI[1].position.y = y;
      playerHealthUI[2].position.x = x;
      playerHealthUI[2].position.y = y;
      local id = createInterface(playerHealthUI, x, y, 10, 0.75, 0.5, 1, 1, false)
      uimanager.register(id, playerHealthUI, {
        function(w, h)
          local newX = w - 10 * 40 - 10
          local newY = 60
          return newX, newY
        end,
        10,
        0.75,
        0.5,
        1,
        1,
        false,
        parent = "playercontrol"
      })
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

  file = io.open(root .. "Entities/" .. name .. ".yaml")

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
