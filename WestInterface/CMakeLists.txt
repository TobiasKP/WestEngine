cmake_minimum_required(VERSION 3.10)
project(WestInterface VERSION 1.0)

# ─────────────────────────────────────────────────────────────
# Packages
# ─────────────────────────────────────────────────────────────
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)

# Collect all source and header files
file(GLOB_RECURSE LIB_SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/*.h
)

# ─────────────────────────────────────────────────────────────
# Building shared Library
# ─────────────────────────────────────────────────────────────
add_library(WestInterface STATIC ${LIB_SOURCES})
target_include_directories(WestInterface PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(WestInterface PUBLIC 
    GLEW::GLEW
    glfw
    WestUtils
)

target_compile_definitions(WestInterface PRIVATE
    $<$<CONFIG:DEBUG>:DEBUG>
)

set(RESOURCE_BASE "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../WestEngine")


# ─────────────────────────────────────────────────────────────
# Additional data copying (scripts, assets, configs, shaders)
# ─────────────────────────────────────────────────────────────
if(NOT EXISTS "${RESOURCE_BASE}/shader")
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_BASE}/shader"
      COMMENT "Creating ${FOLDER} shader directory"
  )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Shader" "${RESOURCE_BASE}/shader"
)


if(NOT EXISTS "${RESOURCE_BASE}/assets/Textures")
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_BASE}/assets/Textures"
      COMMENT "Creating ${FOLDER} texture directory"
  )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Resources" "${RESOURCE_BASE}/assets/Textures"
)
