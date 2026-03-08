Utils = {}

local yaml = require("yaml")
local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^/]+$", "")

local function interpreteData(data, source)
  print(data.name);
  createEntity(data.name);
  addComponent("object", data.model);
  addComponent("shader", data.shader);
  if data.playercontrol then
    addComponent("playercontrol");
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
