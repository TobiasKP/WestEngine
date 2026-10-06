-- Stand-ins for everything the engine injects into the Lua state (LuaFacade.cpp
-- and the systems' registerCFunction calls), plus spy modules for package.preload.
--
-- Events and LogLevel are parsed from the C++ headers so the tests key on the
-- same values the engine exports at startup.

local S = {}

local function read_file(path)
  local f = assert(io.open(path, "r"), "stubs: cannot read " .. path)
  local s = f:read("a")
  f:close()
  return s
end

-- Events: EventIdentifiers in WEST_EVENT_LIST order, value = position (LuaFacade::exportGlobalTables)
local function parse_events()
  local header = read_file(WEST_REPO_ROOT .. "/WestEngine/Constants/InternalEvents.hpp")
  local list = assert(header:match("#define%s+WEST_EVENT_LIST%(X%)(.-)\nenum"), "stubs: WEST_EVENT_LIST not found")
  local events, i = {}, 0
  for name in list:gmatch("X%(([%w_]+)%)") do
    events[name] = i
    i = i + 1
  end
  assert(i > 0, "stubs: no events parsed")
  return events
end

-- LogLevel: enum class Level { Info, Error, Cycle } from WestLogger.h
local function parse_log_levels()
  local header = read_file(WEST_REPO_ROOT .. "/WestUtils/Include/WestLogger.h")
  local body = assert(header:match("enum%s+class%s+Level[^{]*{([^}]*)}"), "stubs: enum Level not found")
  local levels, i = {}, 0
  for name in body:gmatch("([%w_]+)") do
    levels[name] = i
    i = i + 1
  end
  return levels
end

S.Events = parse_events()
S.LogLevel = parse_log_levels()

-- C functions the engine registers as globals (LuaAPI.hpp C_* names)
local ENGINE_FUNCTIONS = {
  "westLog", "dispatchEvent", "createEntity", "addComponent", "buildEntity", "loadWorld",
  "getHealth", "getPosition", "gatherWorldInformation", "aiMoveCommand", "aiAttackCommand",
  "aiEndAction", "getScreenResolution", "createInterface", "updateInterfaceValue",
  "destroyInterface", "getMousePosition",
}

S.calls = {}
S.returns = {}

local function record(name, ...)
  S.calls[name] = S.calls[name] or {}
  table.insert(S.calls[name], table.pack(...))
end

-- List of recorded argument packs for a global or "Module.Function"
function S.calls_of(name)
  return S.calls[name] or {}
end

function S.count(name)
  return #S.calls_of(name)
end

-- Arguments of the most recent call as a plain array (nil if never called)
function S.last(name)
  local c = S.calls_of(name)
  local p = c[#c]
  if p == nil then return nil end
  local out = {}
  for i = 1, p.n do out[i] = p[i] end
  return out
end

local baseline_loaded
local baseline_globals

-- Restores a pristine environment: engine globals reinstalled, call log cleared,
-- every game module unloaded so the next require runs it fresh (the scripts keep
-- their state in module level locals), preload stubs dropped.
function S.reset()
  if baseline_loaded == nil then
    baseline_loaded = {}
    for k, v in pairs(package.loaded) do baseline_loaded[k] = v end
    baseline_globals = {}
    for k in pairs(_G) do baseline_globals[k] = true end
  end
  for k in pairs(package.loaded) do
    if baseline_loaded[k] == nil then package.loaded[k] = nil end
  end
  for k in pairs(package.preload) do package.preload[k] = nil end
  -- globals the scripts defined during the previous test (bare global functions)
  for k in pairs(_G) do
    if not baseline_globals[k] then _G[k] = nil end
  end

  S.calls = {}
  S.returns = {}
  for _, name in ipairs(ENGINE_FUNCTIONS) do
    _G[name] = function(...)
      record(name, ...)
      local r = S.returns[name]
      if r then return r(...) end
    end
  end
  _G.Events = S.Events
  _G.LogLevel = S.LogLevel
  _G.DEBUG = true
end

-- Registers a spy module under package.preload. Every function field (given or
-- auto-created on first access) records its calls as "<modname>.<field>".
-- `fields` may hold plain values (tables, numbers) and real functions; real
-- functions are wrapped so they are recorded too.
function S.spy_module(modname, fields)
  local mod = {}
  for k, v in pairs(fields or {}) do
    if type(v) == "function" then
      mod[k] = function(...)
        record(modname .. "." .. k, ...)
        return v(...)
      end
    else
      mod[k] = v
    end
  end
  setmetatable(mod, {
    __index = function(t, k)
      local f = function(...) record(modname .. "." .. k, ...) end
      rawset(t, k, f)
      return f
    end,
  })
  package.preload[modname] = function() return mod end
  return mod
end

-- Total number of calls recorded on any function of a spy module
function S.module_calls(modname)
  local n = 0
  local prefix = modname .. "."
  for name, list in pairs(S.calls) do
    if name:sub(1, #prefix) == prefix then n = n + #list end
  end
  return n
end

-- Counts westLog calls at a given LogLevel name ("Info", "Error", "Cycle")
function S.logs_at(level_name)
  local n = 0
  for _, p in ipairs(S.calls_of("westLog")) do
    if p[1] == S.LogLevel[level_name] then n = n + 1 end
  end
  return n
end

return S
