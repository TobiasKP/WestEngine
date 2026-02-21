message(STATUS "Building Core Module")

project(WestCore VERSION 0.1.0)

# ─────────────────────────────────────────────────────────────
# Packages
# ─────────────────────────────────────────────────────────────
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(Stb REQUIRED)

# ─────────────────────────────────────────────────────────────
# Includes (shared across platforms)
# ─────────────────────────────────────────────────────────────
message(STATUS "Including headers ...")
include_directories(
    ${LUA_INCLUDE_DIR}
    ${Stb_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/CoreHeaders
    ${CMAKE_CURRENT_SOURCE_DIR}/Core
    ${CMAKE_CURRENT_SOURCE_DIR}/Libs/WEST_INTERFACE
)

# ─────────────────────────────────────────────────────────────
# Platform-specific configuration
# ─────────────────────────────────────────────────────────────
if (WIN32)
    set(PLATFORM "windows")
    set(OPENGL_LIB opengl32)
    add_definitions(-DGLEW_STATIC -DLUA_STATIC) 
elseif(APPLE)
    set(PLATFORM "mac")    
    set(OPENGL_LIB "-framework OpenGL -framework Cocoa -framework IOKit")
elseif(UNIX)
    set(PLATFORM "linux")    
    set(OPENGL_LIB GL)
endif()

# ─────────────────────────────────────────────────────────────
# Link Directories and Targets
# ─────────────────────────────────────────────────────────────
message(STATUS "Linking libraries for platform: ${PLATFORM}")

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    WestUtils
    WestInterface
    GLEW::GLEW
    glfw
    ${LUA_LIBRARIES}
    glm::glm 
    ${OPENGL_LIB}
)

if(UNIX AND NOT APPLE) 
  target_link_libraries(${PROJECT_NAME} 
    PRIVATE 
    m dl pthread  # UNIX common libs
  )
endif()

if(WIN32) 
  target_link_options(${PROJECT_NAME} PRIVATE "-mwindows")
endif()

# ─────────────────────────────────────────────────────────────
# Build Configuration
# ─────────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
)

# ─────────────────────────────────────────────────────────────
# Copy compile_commands.json
# ─────────────────────────────────────────────────────────────
add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/compile_commands.json
)


set(RESOURCE_BASE "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
set(ENGINE_DIR "${RESOURCE_BASE}/engine")

file(GLOB CONFIG_FILES "${CMAKE_SOURCE_DIR}/WestGame/Config/*")

# General file copy post-build
foreach(FOLDER lua assets shader engine config libs libs/STB libs/GLM)
if(NOT EXISTS "${RESOURCE_BASE}/${FOLDER}")
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_BASE}/${FOLDER}"
        COMMENT "Creating ${FOLDER} directory"
    )
endif()
endforeach()

# ─────────────────────────────────────────────────────────────
# Clean old log files before build
# ─────────────────────────────────────────────────────────────

add_custom_command(TARGET ${PROJECT_NAME} PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E remove -f "${RESOURCE_BASE}/WestLog_*.log"
    COMMAND ${CMAKE_COMMAND} -E remove -f "${RESOURCE_BASE}/WestError_*.log"
    COMMAND ${CMAKE_COMMAND} -E remove -f "${RESOURCE_BASE}/WestCyclingLog_*.log"
    COMMENT "Cleaning old WestLogger log files"
)

# ─────────────────────────────────────────────────────────────
# Additional data copying (scripts, assets, configs, shaders)
# ─────────────────────────────────────────────────────────────

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/Constants/AvailableInputCommands.cfg" "${ENGINE_DIR}"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/WestGame/Scripts" "${RESOURCE_BASE}/lua"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/WestGame/Assets" "${RESOURCE_BASE}/assets"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Shader" "${RESOURCE_BASE}/shader"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CONFIG_FILES} "${RESOURCE_BASE}/config"
)

message(STATUS "Configuration complete. Ready to build.")

