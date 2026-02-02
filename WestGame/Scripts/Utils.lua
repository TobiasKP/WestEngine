Utils = {}

local yaml = require("yaml")

local function interpreteData(data)
  local Entity = {
    name = data.name,
    model = data.model,
    shader = data.shader,
    components = {},
    systems = {},
  }

  -- Process components
  if type(data.components) == "table" then
    for _, component in ipairs(data.components) do
      local componentData = {}
      for key, value in pairs(component) do
        componentData[key] = value
      end
      table.insert(Entity.components, componentData)
    end
  end

  -- Process systems
  if type(data.systems) == "table" then
    for _, system in ipairs(data.systems) do
      local systemData = {}
      for key, value in pairs(system) do
        systemData[key] = value
      end
      table.insert(Entity.systems, systemData)
    end
  end

  return Entity
end

function LoadEntity(entity)
  assert(entity ~= nil)

  local file = nil

  file = io.open("lua/Entities/" .. entity .. ".yaml")


  if file == nil then
    print("Error opening file")
    return 1
  end

  local content = file:read("*all")
  file:close()

  local data = yaml.eval(content)
  if data == nil then
    return 1
  end

  return interpreteData(data)
end

Utils.LoadEntity = LoadEntity

return Utils
