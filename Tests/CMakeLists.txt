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

# ─────────────────────────────────────────────────────────────
# Engine source files needed for Tier 2 tests
# ─────────────────────────────────────────────────────────────
set(ENGINE_TEST_SOURCES
    ${CMAKE_SOURCE_DIR}/WestEngine/Core/Entity/World.cpp
    ${CMAKE_SOURCE_DIR}/WestEngine/Core/Entity/Entity.cpp
)

# ─────────────────────────────────────────────────────────────
# Test source files
# ─────────────────────────────────────────────────────────────
set(TEST_SOURCES
    unit/TimeUtilsTest.cpp
    unit/ConfigTest.cpp
    unit/PoolAllocatorTest.cpp
    unit/PathUtilsTest.cpp
    unit/WorldTest.cpp
    unit/EntityTest.cpp
)

# ─────────────────────────────────────────────────────────────
# Test executable
# ─────────────────────────────────────────────────────────────
add_executable(${PROJECT_NAME} ${TEST_SOURCES} ${ENGINE_TEST_SOURCES})

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_SOURCE_DIR}/WestEngine/CoreHeaders
    ${CMAKE_SOURCE_DIR}/WestEngine/Core
    ${CMAKE_SOURCE_DIR}/WestEngine/Constants
    ${CMAKE_SOURCE_DIR}/WestEngine
    ${LUA_INCLUDE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    GTest::gtest
    GTest::gtest_main
    WestUtils
    glm::glm
)

if(WIN32)
    add_definitions(-DGLEW_STATIC -DLUA_STATIC)
endif()

# ─────────────────────────────────────────────────────────────
# Register tests with CTest
# ─────────────────────────────────────────────────────────────
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})
