Utils = {}

local yaml = require("Lib.yaml")
local builder = require("Interface.InterfaceBuilder")
local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^/\\]+$", "")
local projectile = {};
local entityPos = {};
local entityQueue = {};

local function interpreteData(data, qPos)
  local height, width = getScreenResolution();
  if DEBUG then
    print("found resolution: " .. width .. " : " .. height)
  end
  createEntity(data.name);

  addComponent("object", data.model);
  addComponent("shader", data.shader);

  local health = nil
  for i, component in ipairs(data.components) do
    if component.name == "health" then
      health = component
    end
    if component.name == "projectile" and qPos ~= nil then
      data.components[i] = projectile[qPos]
    end
    if component.name == "position" and qPos ~= nil then
      data.components[i] = entityPos[qPos]
    end
  end

  if data.playercontrol then
    addComponent("playercontrol");
    AddPlayerHealth(health);
  end

  if data.activeUnit then
    addComponent("activeUnit")
  end

  -- Process components
  if type(data.components) == "table" then
    for _, component in ipairs(data.components) do
      addComponent(component.name, component);
    end
  end

  if DEBUG then
    print("Creating Entity " .. data.name);
  end

  buildEntity()
end

function AddPlayerHealth(health)
  if health ~= nil then
    builder.Panel("playerhealth")
        :anchor("bottom-right", 415, 60)
        :size(10, 1)
        :alpha(0.5)
        :add(builder.Label(tostring(health.c))
          :color(215, 207, 196, 1.0)
          :span(string.len(tostring(health.c))))
        :add(builder.ProgressBar(100)
          :color(142, 59, 70, 1.0)
          :span(10))
        :build()
  end
end

function loadFile(name)
  local file = nil

  file = io.open(root .. name .. ".yaml")

  if file == nil then
    print("Error opening file")
    return nil
  end

  local content = file:read("*all")
  file:close()

  local data = yaml.eval(content)
  return data
end

function LoadEntity(entity, qPos)
  assert(entity ~= nil)
  local data = loadFile(entity)
  if data == nil then
    return 1
  end

  interpreteData(data, qPos)
end

function world(name)
  assert(name ~= nil)
  local data = loadFile(name)
  if data == nil then
    return 1
  end
  loadWorld(data.world);
end

function FillProjectileInfo(speed, target, dmg, hit, x, z)
  table.insert(projectile, { name = "projectile", speed = speed, destination = target, dmg = dmg, hit = hit })
  table.insert(entityPos, { name = "position", x = x, y = 0, z = z })
end

function QueueEntity(path)
  table.insert(entityQueue, path)
end

function DrainQueue()
  local localQ = entityQueue
  entityQueue = {}
  for i, path in ipairs(localQ) do
    LoadEntity(path, i)
  end
  projectile = {}
  entityPos = {}
end

Utils.LoadEntity = LoadEntity
Utils.FillProjectileInfo = FillProjectileInfo
Utils.QueueEntity = QueueEntity
Utils.DrainQueue = DrainQueue
Utils.World = world;

return Utils
