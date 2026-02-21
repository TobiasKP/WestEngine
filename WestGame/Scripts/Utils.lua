Utils = {}

local yaml = require("yaml")
local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^/]+$", "")

local function interpreteData(data, source)
  local log = "YAML parsed for " .. tostring(source)
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

  -- Process World
  if type(data.world) == "table" and #data.world > 0 then
    Entity.world = {}
    for _, details in ipairs(data.world) do
      local detailData = {}
      for key, value in pairs(details) do
        detailData[key] = value
        log = log .. " | " .. tostring(key) .. "=" .. type(value)
      end
      table.insert(Entity.world, detailData)
    end
    log = log .. " | world entries: " .. #Entity.world
  end

  log = log .. " | components: " .. #Entity.components .. " | systems: " .. #Entity.systems
  Entity._parseLog = log

  return Entity
end

function LoadEntity(entity)
  assert(entity ~= nil)

  local file = nil

  file = io.open(root .. "Entities/" .. entity .. ".yaml")


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

  return interpreteData(data, entity)
end

Utils.LoadEntity = LoadEntity

return Utils
