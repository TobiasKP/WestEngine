cmake_minimum_required(VERSION 3.10)
project(WestInterface VERSION 1.0)

# Collect all source and header files
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# === Shared Library ===
add_library(WestInterface SHARED ${LIB_SOURCES})
target_include_directories(WestInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(WestInterface PUBLIC WestUtils)

if(WIN32)
    target_compile_definitions(WestInterface PRIVATE WEST_INTERFACE_EXPORTS)
endif()

