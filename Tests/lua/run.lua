-- Entry point: lua run.lua <repo root> <test file>
-- Sets package.path so tests can require the harness/stubs from Tests/lua and the
-- real game modules from WestGame/Scripts (same layout LuaFacade uses at runtime).

local repo_root, test_file = arg[1], arg[2]
if not repo_root or not test_file then
  io.stderr:write("usage: lua run.lua <repo root> <test file>\n")
  os.exit(2)
end

WEST_REPO_ROOT    = repo_root
WEST_SCRIPTS_DIR  = repo_root .. "/WestGame/Scripts"

package.path = repo_root .. "/Tests/lua/?.lua;"
    .. WEST_SCRIPTS_DIR .. "/?.lua;"
    .. WEST_SCRIPTS_DIR .. "/?/init.lua;"
    .. package.path

local T = require("harness")
local chunk, err = loadfile(test_file)
if not chunk then
  io.stderr:write("cannot load " .. test_file .. ": " .. tostring(err) .. "\n")
  os.exit(2)
end
chunk()

local label = test_file:match("([^/\\]+)%.lua$") or test_file
os.exit(T.run(label) and 0 or 1)
