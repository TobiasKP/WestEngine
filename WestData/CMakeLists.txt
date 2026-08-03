message(STATUS "Building Data Module")

project(WestData VERSION 0.1.0)

# ─────────────────────────────────────────────────────────────
# Packages
# ─────────────────────────────────────────────────────────────
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)

# ─────────────────────────────────────────────────────────────
# Sources
# ─────────────────────────────────────────────────────────────
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# ─────────────────────────────────────────────────────────────
# Building static Library
# ─────────────────────────────────────────────────────────────
add_library(WestData STATIC ${LIB_SOURCES})

target_include_directories(WestData
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:include>"
    PRIVATE
        ${Stb_INCLUDE_DIR}
)

target_link_libraries(WestData PUBLIC
    assimp::assimp
    WestUtils
)

target_compile_definitions(WestData PRIVATE
    $<$<CONFIG:DEBUG>:DEBUG>
)

