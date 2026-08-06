Utils = {}

local yaml = require("Lib.yaml")

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

  for i, component in ipairs(data.components) do
    if component.name == "projectile" and qPos ~= nil then
      data.components[i] = projectile[qPos]
    end
    if component.name == "position" and qPos ~= nil then
      data.components[i] = entityPos[qPos]
    end
  end

  if data.playercontrol == true then
    addComponent("playercontrol");
  elseif data.playercontrol == false then
    addComponent("aicontrol");
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

function loadFile(name)
  local file = io.open(root .. name .. ".yaml")

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

function FillProjectileInfo(attacker, speed, target, dmg, hit, x, z)
  table.insert(projectile,
    { name = "projectile", attacker = attacker, speed = speed, destination = target, dmg = dmg, hit = hit })
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
