-- Core/EntityUtils.lua: SpawnMisc turns the world's miscPositions into plain ECS entities.
local T = require("harness")
local S = require("stubs")

local Utils

local GRID = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }

local function world(misc, grid)
  return { world = { { skybox = "skybox" }, { grid = grid or GRID } }, miscPositions = misc }
end

local function components_named(name)
  local out = {}
  for _, p in ipairs(S.calls_of("addComponent")) do
    if p[1] == name then out[#out + 1] = p[2] end
  end
  return out
end

T.before_each(function()
  S.reset()
  Utils = require("Core.EntityUtils")
end)

T.test("a tree on a 4x4 grid is placed in the tile centre", function()
  Utils.SpawnMisc(world({ { x = 1, y = 2, type = "broadleaf_01" } }))
  T.eq(S.count("createEntity"), 1)
  T.eq(S.last("createEntity"), { "broadleaf_01" })
  T.eq(components_named("object"), { "broadleaf_01.obj" })
  T.eq(components_named("shader"), { { v = "Vertex.vs", f = "Fragment.fs" } })
  T.eq(components_named("position"), { { name = "position", x = -0.5, y = 0, z = 0.5 } })
  T.eq(S.count("buildEntity"), 1)
  T.eq(S.logs_at("Error"), 0)
end)

T.test("the tile height becomes the position y", function()
  local grid = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 0, 0, 0, 0, 0, 0 }
  Utils.SpawnMisc(world({ { x = 1, y = 2, type = "broadleaf_01" } }, grid))
  T.eq(components_named("position")[1].y, 3)
end)

T.test("out of bounds, fractional or untyped entries are skipped with an error", function()
  local bad = {
    { { x = 4, y = 0, type = "broadleaf_01" } },
    { { x = 0, y = -1, type = "broadleaf_01" } },
    { { x = 0.5, y = 0, type = "broadleaf_01" } },
    { { x = 0, y = 0 } },
    { { x = 0, y = 0, type = "" } },
  }
  for _, misc in ipairs(bad) do
    S.reset()
    Utils = require("Core.EntityUtils")
    Utils.SpawnMisc(world(misc))
    T.eq(S.count("createEntity"), 0)
    T.eq(S.count("buildEntity"), 0)
    T.eq(S.logs_at("Error"), 1)
  end
end)

T.test("a second object on the same tile is skipped with an error", function()
  Utils.SpawnMisc(world({ { x = 2, y = 1, type = "broadleaf_01" }, { x = 2, y = 1, type = "broadleaf_01" } }))
  T.eq(S.count("createEntity"), 1)
  T.eq(S.count("buildEntity"), 1)
  T.eq(S.logs_at("Error"), 1)
end)

T.test("a world without miscPositions spawns nothing", function()
  Utils.SpawnMisc(world(nil))
  T.eq(S.count("createEntity"), 0)
  T.eq(S.count("addComponent"), 0)
  T.eq(S.count("buildEntity"), 0)
end)

T.test("misc objects are not units", function()
  Utils.SpawnMisc(world({ { x = 1, y = 2, type = "broadleaf_01" } }))
  for _, p in ipairs(S.calls_of("addComponent")) do
    T.truthy(p[1] ~= "activeUnit" and p[1] ~= "playercontrol" and p[1] ~= "aicontrol", p[1])
  end
end)

T.test("World loads the grid and then spawns the misc objects", function()
  Utils.World("../Entities/World/DemoWorld")
  T.eq(S.count("loadWorld"), 1)
  T.eq(S.count("createEntity"), 2)
  T.eq(S.count("buildEntity"), 2)
end)

T.test("DemoWorld.yaml lists miscPositions as tables", function()
  local f = assert(io.open(WEST_SCRIPTS_DIR .. "/Entities/World/DemoWorld.yaml"))
  local data = require("Lib.yaml").eval(f:read("a"))
  f:close()
  T.eq(type(data.miscPositions), "table")
  T.truthy(#data.miscPositions > 0)
  for _, misc in ipairs(data.miscPositions) do
    T.eq(type(misc), "table")
    T.eq(math.type(misc.x), "integer")
    T.eq(math.type(misc.y), "integer")
    T.eq(type(misc.type), "string")
  end
end)
