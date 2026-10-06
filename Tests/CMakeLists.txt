message(STATUS "Building Test Module")

project(WestTests VERSION 0.1.0)

# ─────────────────────────────────────────────────────────────
# Packages
# ─────────────────────────────────────────────────────────────
find_package(GTest CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW REQUIRED)
find_package(Lua REQUIRED)

# ─────────────────────────────────────────────────────────────
# WestUtils (already a static library from parent)
# ─────────────────────────────────────────────────────────────
if(NOT TARGET WestUtils)
  add_subdirectory(${CMAKE_SOURCE_DIR}/WestUtils ${CMAKE_BINARY_DIR}/WestUtils)
endif()

if(NOT TARGET WestData)
  add_subdirectory(${CMAKE_SOURCE_DIR}/WestData ${CMAKE_BINARY_DIR}/WestData)
endif()

# ─────────────────────────────────────────────────────────────
# Engine source files needed for Tier 2 tests
# ─────────────────────────────────────────────────────────────
set(ENGINE_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/WestEngine/Core/Entity/World.cpp
    ${CMAKE_SOURCE_DIR}/WestEngine/Core/Entity/Entity.cpp
    ${CMAKE_SOURCE_DIR}/WestEngine/Core/Events/EventDispatcher.cpp
    ${CMAKE_SOURCE_DIR}/WestEngine/Core/Events/EventPayload.cpp
    ${CMAKE_SOURCE_DIR}/WestInterface/WestInterface/RenderManagment/ComponentDataPool.cpp
)

# EventDispatcher::init() references the LuaFacade singleton; the tests never call
# it, so a two-symbol stub replaces LuaFacade.cpp at link time.
set(TEST_SUPPORT_SOURCES
    unit/support/LuaFacadeLinkStub.cpp
)

# ─────────────────────────────────────────────────────────────
# Test source files
# ─────────────────────────────────────────────────────────────
set(TEST_SOURCES
    unit/TimeUtilsTest.cpp
    unit/ConfigTest.cpp
    unit/ConfigIdReuseTest.cpp
    unit/PoolAllocatorTest.cpp
    unit/PathUtilsTest.cpp
    unit/WorldTest.cpp
    unit/WorldFlagTest.cpp
    unit/WorldVisibilityTest.cpp
    unit/EntitySemanticsTest.cpp
    unit/ComponentArrayTest.cpp
    unit/ThreadSafeQueueTest.cpp
    unit/ECSStressTest.cpp
    unit/DataPoolTest.cpp
    unit/ModelTest.cpp
    unit/AssetImporterConcurrencyTest.cpp
    unit/EventDispatcherTest.cpp
    unit/EventPayloadLuaTest.cpp
    unit/ComponentDataPoolTest.cpp
)

# ─────────────────────────────────────────────────────────────
# Test executable
# ─────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${TEST_SOURCES} ${ENGINE_TEST_SOURCES} ${TEST_SUPPORT_SOURCES})

# Shared by the unit and perf executables
function(west_configure_test_target target)
  target_include_directories(${target} PRIVATE
      ${CMAKE_SOURCE_DIR}/WestEngine/CoreHeaders
      ${CMAKE_SOURCE_DIR}/WestEngine/Core
      ${CMAKE_SOURCE_DIR}/WestEngine/Constants
      ${CMAKE_SOURCE_DIR}/WestEngine
      ${CMAKE_SOURCE_DIR}/WestData
      ${CMAKE_SOURCE_DIR}/WestInterface/WestInterface
      ${LUA_INCLUDE_DIR}
  )

  target_link_libraries(${target} PRIVATE
      GTest::gtest
      GTest::gtest_main
      WestUtils
      WestData
      glm::glm
      ${LUA_LIBRARIES}
  )

  target_compile_definitions(${target} PRIVATE
      WEST_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
      # Bounds-checked operator[] and friends: out-of-range access in code under
      # test aborts instead of silently reading neighbouring memory.
      _GLIBCXX_ASSERTIONS
  )
endfunction()

west_configure_test_target(${PROJECT_NAME})

if(WIN32)
    add_definitions(-DGLEW_STATIC -DLUA_STATIC)
endif()

# ─────────────────────────────────────────────────────────────
# Register tests with CTest
# ─────────────────────────────────────────────────────────────
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME}
    PROPERTIES LABELS unit TIMEOUT 60
)

# ─────────────────────────────────────────────────────────────
# Performance tests (wall-clock budgets, machine dependent)
#
# Always built so they keep compiling, but only registered with ctest on request:
#   cmake -DWEST_PERF_TESTS=ON <build dir> && ctest -L perf
# or run <build dir>/Tests/WestPerfTests directly.
# ─────────────────────────────────────────────────────────────
option(WEST_PERF_TESTS "Register the timing tests (label perf) with ctest" OFF)

add_executable(WestPerfTests perf/ECSPerfTest.cpp ${CMAKE_SOURCE_DIR}/WestEngine/Core/Entity/World.cpp
    ${CMAKE_SOURCE_DIR}/WestEngine/Core/Entity/Entity.cpp)
west_configure_test_target(WestPerfTests)

if(WEST_PERF_TESTS)
  gtest_discover_tests(WestPerfTests
      PROPERTIES LABELS perf RUN_SERIAL TRUE
  )
endif()

# ─────────────────────────────────────────────────────────────
# Lua unit tests for WestGame/Scripts (Tests/lua, self-contained harness)
#
#   ctest -L lua-unit                      run them
#   WEST_LUA_RUN_SKIPPED=1 ctest -L lua-unit   also run the skipped (known bug) cases
# ─────────────────────────────────────────────────────────────
find_program(WEST_LUA_INTERPRETER NAMES lua5.4 lua54 lua)

if(WEST_LUA_INTERPRETER)
  file(GLOB WEST_LUA_TEST_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/lua/test_*.lua")
  foreach(test_file ${WEST_LUA_TEST_FILES})
    get_filename_component(test_name ${test_file} NAME_WE)
    string(REGEX REPLACE "^test_" "" test_name ${test_name})
    add_test(NAME lua.unit.${test_name}
        COMMAND ${WEST_LUA_INTERPRETER} ${CMAKE_CURRENT_SOURCE_DIR}/lua/run.lua ${CMAKE_SOURCE_DIR} ${test_file}
    )
    set_tests_properties(lua.unit.${test_name} PROPERTIES
        LABELS "lua;lua-unit"
        TIMEOUT 30
    )
  endforeach()
else()
  message(STATUS "Lua interpreter not found - Lua unit tests (Tests/lua) not registered")
endif()
