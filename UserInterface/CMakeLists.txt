cmake_minimum_required(VERSION 3.10)
project(WestInterface VERSION 1.0)

# Collect all source and header files
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# Headers for external use
set(WEST_INTERFACE_HEADERS "${CMAKE_SOURCE_DIR}/WestInterfaceFacade.h")

# === Shared Library ===
add_library(WestInterface SHARED ${LIB_SOURCES})
target_include_directories(WestInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(WestInterface PROPERTIES VERSION ${PROJECT_VERSION} SOVERSION 1 OUTPUT_NAME "WestInterface")

if(WIN32)
    target_compile_definitions(WestInterface PRIVATE WEST_INTERFACE_EXPORTS)
endif()

# === Static Library ===
add_library(WestInterfaceStatic STATIC ${LIB_SOURCES})
target_include_directories(WestInterfaceStatic PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
set_target_properties(WestInterfaceStatic PROPERTIES OUTPUT_NAME "WestInterface")

# === Determine Output Directory ===
if(WIN32)
    set(OUT_DIR "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/WEST_INTERFACE/windows")
elseif(APPLE)
    set(OUT_DIR "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/WEST_INTERFACE/mac")
else()
    set(OUT_DIR "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/WEST_INTERFACE/linux")
endif()

# === Create output directory ===
add_custom_command(TARGET WestInterface POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${OUT_DIR}"
)

# === Copy Shared Library ===
add_custom_command(TARGET WestInterface POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:WestInterface>"
        "${OUT_DIR}/"
    COMMENT "Copying shared library to ${OUT_DIR}"
)

# === Copy Static Library ===
add_custom_command(TARGET WestInterfaceStatic POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "$<TARGET_FILE:WestInterfaceStatic>"
        "${OUT_DIR}/"
    COMMENT "Copying static library to ${OUT_DIR}"
)

# === Copy Header ===
add_custom_command(TARGET WestInterface POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${WEST_INTERFACE_HEADERS}
        "${OUT_DIR}/../"
    COMMENT "Copying header file to Libs/WEST_INTERFACE"
)

