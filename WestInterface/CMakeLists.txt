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

target_compile_definitions(WestInterface PRIVATE
    $<$<CONFIG:DEBUG>:DEBUG>
)

set(RESOURCE_BASE "$<TARGET_FILE_DIR:${PROJECT_NAME}>/../WestEngine")

if(NOT EXISTS "${RESOURCE_BASE}/shader")
  add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
      COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_BASE}/shader"
      COMMENT "Creating ${FOLDER} directory"
  )
endif()

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Shader" "${RESOURCE_BASE}/shader"
)


