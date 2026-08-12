-- luacheck configuration for WestGame/Scripts
--
-- The C++ <-> Lua contract lives in WestEngine/Constants/LuaAPI.hpp, so this
-- config parses that header instead of duplicating the names here:
--
--   C_* constants   -> functions C++ registers into the Lua state (read-only here)
--   other constants -> functions Lua must define, because C++ calls them
--
-- Adding a binding in LuaAPI.hpp therefore teaches luacheck about it
-- automatically, and removing one turns stale Lua call sites into warnings.
--
-- Run via CMake (`cmake --build <dir> --target lint-lua`), ctest (`-R lua`)
-- or directly from this folder: `luacheck Scripts`

-- Resolve paths from this file's own location so luacheck can be invoked from
-- any working directory (build tree, editor, CI).
local config_dir = debug.getinfo(1, "S").source:match("^@(.*)[/\\][^/\\]*$") or "."
local repo_root = config_dir .. "/.."

local function parse_lua_api(header)
  local lua_defined, cpp_defined = {}, {}
  local file = assert(io.open(header, "r"), "luacheck config: cannot read " .. header)

  for line in file:lines() do
    local const, name = line:match('constexpr%s+std::string_view%s+([%w_]+)%s*=%s*"([%w_]+)"')
    if const then
      table.insert(const:match("^C_") and cpp_defined or lua_defined, name)
    end
  end

  file:close()
  assert(#lua_defined > 0 and #cpp_defined > 0, "luacheck config: no bindings parsed from " .. header)
  return lua_defined, cpp_defined
end

local lua_entry_points, engine_bindings = parse_lua_api(repo_root .. "/WestEngine/Constants/LuaAPI.hpp")

-- Entry points called from C++ but not declared in LuaAPI.hpp.
-- SceneManager.cpp:95/105 and LuaFacade.cpp:169 use string literals.
table.insert(lua_entry_points, "Init")
table.insert(lua_entry_points, "LoadScene")

-- Injected by LuaFacade::startup via luaL_dostring (LuaFacade.cpp:52/54).
table.insert(engine_bindings, "DEBUG")

std = "lua54"
codes = true
max_line_length = 120

globals = lua_entry_points
read_globals = engine_bindings

-- The scripts use a module pattern that declares functions as bare globals and
-- then attaches them to a module table (see EntityUtils.lua, GameState.lua).
-- Accept it so the linter is adoptable today; typos in engine calls and in
-- forward references are still caught.
-- Note: this also suppresses W121/W122 (shadowing a Lua standard global), which
-- the `lua.no_stdlib_shadowing` pass in CMakeLists.txt restores.
allow_defined = true

-- W131 (unused global): every global declared above is consumed by C++, never by
-- Lua, so luacheck cannot tell used from unused and would fire on every entry point.
ignore = { "131" }

-- Vendored third-party code.
exclude_files = { "Scripts/Lib" }
