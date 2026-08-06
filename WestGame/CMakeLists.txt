message(STATUS "Configuring Game Scripts")

# ─────────────────────────────────────────────────────────────
# Lua static analysis (luacheck)
#
# Rules live in WestGame/.luacheckrc, which derives the C++ <-> Lua global
# contract from WestEngine/Constants/LuaAPI.hpp instead of duplicating it.
#
#   cmake --build <dir> --target lint-lua    run the linter
#   ctest -R lua                             run it as a test (needs BUILD_TESTS)
#   -DWEST_LINT_LUA=OFF                      drop the targets entirely
#
# Debug builds treat findings as build errors (WEST_LINT_LUA_STRICT defaults to ON
# for CMAKE_BUILD_TYPE=Debug), so scripts are linted as part of every build there.
# Release builds only lint on request. Passing -DWEST_LINT_LUA_STRICT=<ON|OFF>
# explicitly overrides the default either way.
#
# luacheck is an optional tool: when it is missing the targets are skipped with a
# status message, so builds keep working on machines without it.
# ─────────────────────────────────────────────────────────────

string(TOLOWER "${CMAKE_BUILD_TYPE}" WEST_BUILD_TYPE_LOWER)

if(WEST_BUILD_TYPE_LOWER STREQUAL "debug")
    set(WEST_LINT_LUA_STRICT_DEFAULT ON)
else()
    set(WEST_LINT_LUA_STRICT_DEFAULT OFF)
endif()

option(WEST_LINT_LUA "Enable Lua static analysis via luacheck" ON)
option(WEST_LINT_LUA_STRICT "Treat Lua lint findings as build errors" ${WEST_LINT_LUA_STRICT_DEFAULT})

if(NOT WEST_LINT_LUA)
    return()
endif()

find_program(LUACHECK_EXECUTABLE NAMES luacheck)

if(NOT LUACHECK_EXECUTABLE)
    if(WEST_LINT_LUA_STRICT)
        # Strict was asked for (default in Debug) but cannot be honoured: without
        # this warning the build would silently come out ungated.
        message(WARNING "luacheck not found - Lua lint gate NOT installed, scripts are unchecked.\n"
                "Install it (Arch: pacman -S luacheck, otherwise: luarocks install luacheck),\n"
                "then re-run cmake to pick it up - or set -DWEST_LINT_LUA=OFF.")
    else()
        message(STATUS "luacheck not found - Lua linting disabled "
                "(install: pacman -S luacheck / luarocks install luacheck)")
    endif()
    return()
endif()

message(STATUS "luacheck found: ${LUACHECK_EXECUTABLE}")
message(STATUS "Lua lint gates the build: ${WEST_LINT_LUA_STRICT} (build type: ${CMAKE_BUILD_TYPE})")

# Everything runs with this folder as the working directory, so luacheck picks up
# WestGame/.luacheckrc and reports paths relative to WestGame.
file(GLOB_RECURSE WEST_LUA_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/Scripts/*.lua")

# Pass 1 - full rule set from .luacheckrc.
set(WEST_LUACHECK_CMD ${LUACHECK_EXECUTABLE} Scripts)

# Pass 2 - .luacheckrc allows the "bare global function" module pattern the
# scripts use, which as a side effect also silences W121/W122. This pass runs
# without that allowance and reports only those two codes, so a `function load()`
# clobbering a Lua standard library global in the shared state is still caught.
set(WEST_LUACHECK_SHADOW_CMD ${LUACHECK_EXECUTABLE} --no-config --std lua54
        --exclude-files Scripts/Lib --only 121 122 -- Scripts)

add_custom_target(lint-lua
        COMMAND ${WEST_LUACHECK_CMD}
        COMMAND ${WEST_LUACHECK_SHADOW_CMD}
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        SOURCES ${WEST_LUA_SOURCES}
        COMMENT "luacheck: WestGame/Scripts"
        VERBATIM
)

if(BUILD_TESTS)
    add_test(NAME lua.luacheck COMMAND ${WEST_LUACHECK_CMD})
    add_test(NAME lua.no_stdlib_shadowing COMMAND ${WEST_LUACHECK_SHADOW_CMD})

    set_tests_properties(lua.luacheck lua.no_stdlib_shadowing PROPERTIES
            WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
            LABELS lua
    )
endif()

# Opt-in gate: lint before WestCore builds, so broken scripts never reach the
# POST_BUILD copy into <bindir>/lua.
if(WEST_LINT_LUA_STRICT)
    if(TARGET WestCore)
        add_dependencies(WestCore lint-lua)
    else()
        message(WARNING "WEST_LINT_LUA_STRICT=ON but target WestCore does not exist - gate not installed")
    endif()
endif()
