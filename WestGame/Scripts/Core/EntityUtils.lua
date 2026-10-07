Utils = {}

local yaml = require("Lib.yaml")

local root = debug.getinfo(1, 'S').source:sub(2):gsub("[^/\\]+$", "")
local projectile = {};
local entityPos = {};
local entityQueue = {};

local function interpreteData(data, qPos)
  local height, width = getScreenResolution();
  if DEBUG then
    westLog(LogLevel.Cycle, "Screen resolution for entity creation: " .. tostring(width) .. " x " .. tostring(height))
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
    westLog(LogLevel.Info, "Creating entity: " .. tostring(data.name))
  end

  buildEntity()
end

function loadFile(name)
  local file = io.open(root .. name .. ".yaml")

  if file == nil then
    westLog(LogLevel.Error, "Error opening entity file: " .. tostring(root) .. tostring(name) .. ".yaml")
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

local function spawnMisc(data)
  local grid = {}
  for _, entry in ipairs(data.world or {}) do
    grid = entry.grid or grid
  end
  local dim, seen = math.sqrt(#grid), {}
  for _, misc in ipairs(data.miscPositions or {}) do
    local x, y, kind = misc.x, misc.y, misc.type
    local onGrid = type(x) == "number" and type(y) == "number" and x % 1 == 0 and y % 1 == 0
      and x >= 0 and y >= 0 and x < dim and y < dim
    local tile = onGrid and y * dim + x + 1 or nil
    if tile == nil or type(kind) ~= "string" or kind == "" or seen[tile] then
      westLog(LogLevel.Error, "Skipping misc object " .. tostring(kind) .. " at " .. tostring(x) .. ", " .. tostring(y))
    else
      seen[tile] = true
      createEntity(kind)
      addComponent("object", kind .. ".obj")
      addComponent("shader", { v = "Vertex.vs", f = "Fragment.fs" })
      addComponent("position", { name = "position", x = x + 0.5 - dim / 2, y = grid[tile], z = y + 0.5 - dim / 2 })
      buildEntity()
    end
  end
end

function world(name)
  assert(name ~= nil)
  local data = loadFile(name)
  if data == nil then
    return 1
  end
  loadWorld(data.world);
  spawnMisc(data)
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
Utils.SpawnMisc = spawnMisc

return Utils
