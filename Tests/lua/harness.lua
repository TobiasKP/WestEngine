-- Minimal self-contained test harness for the WestGame Lua scripts.
--
--   local T = require("harness")
--   T.before_each(function() ... end)
--   T.test("name", function() T.eq(actual, expected) end)
--   -- BUG: <file>:<line> <explanation>
--   T.skip("name", "BUG: <file>:<line> <explanation>", function() ... end)
--
-- Skipped tests document confirmed production bugs. They are reported but not
-- run; set WEST_LUA_RUN_SKIPPED=1 to run them anyway (they are expected to fail).
-- run.lua executes one test file and exits non-zero if any test failed.

local T = {}

local tests = {}
local hooks = {}

function T.test(name, fn)
  tests[#tests + 1] = { name = name, fn = fn }
end

function T.skip(name, reason, fn)
  tests[#tests + 1] = { name = name, fn = fn, skip = reason }
end

function T.before_each(fn)
  hooks[#hooks + 1] = fn
end

-- ─── Assertions ────────────────────────────────────────────

local function fmt(v)
  if type(v) == "string" then
    return string.format("%q", v)
  end
  if type(v) ~= "table" then
    return tostring(v)
  end
  local keys = {}
  for k in pairs(v) do keys[#keys + 1] = k end
  table.sort(keys, function(a, b) return tostring(a) < tostring(b) end)
  local parts = {}
  for _, k in ipairs(keys) do
    parts[#parts + 1] = tostring(k) .. "=" .. fmt(v[k])
  end
  return "{" .. table.concat(parts, ", ") .. "}"
end

local function deep_equal(a, b)
  if type(a) ~= type(b) then return false end
  if type(a) ~= "table" then return a == b end
  for k, v in pairs(a) do
    if not deep_equal(v, b[k]) then return false end
  end
  for k in pairs(b) do
    if a[k] == nil then return false end
  end
  return true
end

local function fail(msg, level)
  error(msg, (level or 1) + 2)
end

function T.eq(actual, expected, msg)
  if not deep_equal(actual, expected) then
    fail(string.format("expected %s, got %s%s", fmt(expected), fmt(actual), msg and (" -- " .. msg) or ""))
  end
end

function T.ne(actual, unexpected, msg)
  if deep_equal(actual, unexpected) then
    fail(string.format("did not expect %s%s", fmt(actual), msg and (" -- " .. msg) or ""))
  end
end

function T.truthy(v, msg)
  if not v then
    fail("expected a truthy value, got " .. fmt(v) .. (msg and (" -- " .. msg) or ""))
  end
end

function T.falsy(v, msg)
  if v then
    fail("expected a falsy value, got " .. fmt(v) .. (msg and (" -- " .. msg) or ""))
  end
end

function T.no_error(fn, msg)
  local ok, err = pcall(fn)
  if not ok then
    fail("unexpected error: " .. tostring(err) .. (msg and (" -- " .. msg) or ""))
  end
end

-- ─── Runner ────────────────────────────────────────────────

function T.run(file_label)
  local run_skipped = os.getenv("WEST_LUA_RUN_SKIPPED") == "1"
  local passed, failed, skipped = 0, 0, 0
  local failures = {}

  print(string.format("[==========] %d tests from %s", #tests, file_label))
  for _, t in ipairs(tests) do
    if t.skip and not run_skipped then
      skipped = skipped + 1
      print("[ SKIPPED  ] " .. t.name .. "  (" .. t.skip .. ")")
    else
      local ok, err = xpcall(function()
        for _, h in ipairs(hooks) do h() end
        t.fn()
      end, debug.traceback)
      if ok then
        passed = passed + 1
        print("[       OK ] " .. t.name)
      else
        failed = failed + 1
        failures[#failures + 1] = t.name
        print("[  FAILED  ] " .. t.name .. (t.skip and "  (skipped test, run on request)" or ""))
        print(err)
      end
    end
  end

  print(string.format("[==========] %d passed, %d failed, %d skipped", passed, failed, skipped))
  for _, name in ipairs(failures) do
    print("[  FAILED  ] " .. name)
  end
  return failed == 0
end

return T
