message(STATUS "Building Render Module")

project(WestRenderer VERSION 0.1.0)

# ─────────────────────────────────────────────────────────────
# Packages
# ─────────────────────────────────────────────────────────────

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
add_library(WestRenderer STATIC ${LIB_SOURCES})

target_include_directories(WestRenderer
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<INSTALL_INTERFACE:include>"
    PRIVATE
        ${Stb_INCLUDE_DIR}
)

target_link_libraries(WestRenderer PUBLIC 
    WestUtils
    WestData
    WestInterface
)

target_compile_definitions(WestRenderer PRIVATE
    $<$<CONFIG:DEBUG>:DEBUG>
)

