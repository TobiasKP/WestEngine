cmake_minimum_required(VERSION 3.10)
project(WestEngine VERSION 1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "C++ Standard = ${CMAKE_CXX_STANDARD}")

# ─────────────────────────────────────────────────────────────
# Includes (shared across platforms)
# ─────────────────────────────────────────────────────────────
message(STATUS "Including headers ...")
include_directories(
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/CoreHeaders
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/GLEW/include
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/GLFW/include
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/LUA/include
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/STB
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/GLM
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/WEST_INTERFACE
)

# ─────────────────────────────────────────────────────────────
# Platform-specific configuration
# ─────────────────────────────────────────────────────────────
if (WIN32)
    set(PLATFORM "windows")
    set(GLEW_LIB glew32s)
    set(GLFW_LIB glfw3)
    set(LUA_LIB lua54)
  #  set(WEST_INTERFACE_LIB WestInterface)
    set(OPENGL_LIB opengl32)
    add_definitions(-DGLEW_STATIC -DLUA_STATIC)
    target_link_options(${PROJECT_NAME} PRIVATE "-mwindows")
elseif(APPLE)
    set(PLATFORM "mac")
    set(GLEW_LIB GLEW)
    set(GLFW_LIB glfw)
    set(LUA_LIB lua)
  # set(WEST_INTERFACE_LIB WestInterface)
    set(OPENGL_LIB "-framework OpenGL -framework Cocoa -framework IOKit")
elseif(UNIX)
    set(PLATFORM "linux")
    set(GLEW_LIB GLEW)
    set(GLFW_LIB glfw)
    set(LUA_LIB lua)
   #set(WEST_INTERFACE_LIB WestInterface)
    set(OPENGL_LIB GL)
endif()

# ─────────────────────────────────────────────────────────────
# Link Directories and Targets
# ─────────────────────────────────────────────────────────────
message(STATUS "Linking libraries for platform: ${PLATFORM}")
link_directories(
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/GLEW/${PLATFORM}/lib
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/GLFW/${PLATFORM}/lib
    ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/LUA/${PLATFORM}/lib
  #  ${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/WEST_INTERFACE/${PLATFORM}/lib
)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_SOURCE_DIR}/../WestEngine/Core/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME}
    ${GLEW_LIB}
    ${GLFW_LIB}
    ${LUA_LIB}
  #  ${WEST_INTERFACE_LIB}
    ${OPENGL_LIB}
    m dl pthread  # UNIX common libs
)

# ─────────────────────────────────────────────────────────────
# Build Configuration
# ─────────────────────────────────────────────────────────────
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release)

target_compile_definitions(${PROJECT_NAME} PRIVATE
    $<$<CONFIG:DEBUG>:DEBUG>
)

# ─────────────────────────────────────────────────────────────
# Copy compile_commands.json
# ─────────────────────────────────────────────────────────────
add_custom_target(copy_compile_commands ALL
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    ${CMAKE_BINARY_DIR}/compile_commands.json
    ${CMAKE_SOURCE_DIR}/../compile_commands.json
)


set(RESOURCE_BASE "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
set(ENGINE_DIR "${RESOURCE_BASE}/engine")

file(GLOB CONFIG_FILES "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Game/Config/*")
file(GLOB STB_FILES "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/STB/*")
file(GLOB GLM_FILES "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/GLM/*")

# General file copy post-build
foreach(FOLDER lua assets shader engine config libs libs/STB libs/GLM)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_BASE}/${FOLDER}"
        COMMENT "Creating ${FOLDER} directory"
    )
endforeach()

# ─────────────────────────────────────────────────────────────
# Post-build file copying 
# ─────────────────────────────────────────────────────────────
if (WIN32)
    foreach(DLL_NAME
        glew32.dll
        glfw3.dll
        lua54.dll
        assimp.dll
  #      WestInterface.dll
    )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/*/${PLATFORM}/lib/${DLL_NAME}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${DLL_NAME} to build output"
        )
    endforeach()
elseif(UNIX)
    foreach(SO_NAME
        libGLEW.so.2.2.0
        libglfw.so.3.3
        liblua.so
        libassimp.so
    )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Libs/*/${PLATFORM}/lib/${SO_NAME}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${SO_NAME} to build output"
        )
    endforeach()
endif()

# ─────────────────────────────────────────────────────────────
# Additional data copying (scripts, assets, configs, shaders)
# ─────────────────────────────────────────────────────────────

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Constants/AvailableInputCommands.cfg" "${ENGINE_DIR}"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Game/Scripts" "${RESOURCE_BASE}/lua"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Game/Assets" "${RESOURCE_BASE}/assets"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_SOURCE_DIR}/../WestEngine/Core/Shader" "${RESOURCE_BASE}/shader"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CONFIG_FILES} "${RESOURCE_BASE}/config"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${STB_FILES} "${RESOURCE_BASE}/libs/STB"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${GLM_FILES} "${RESOURCE_BASE}/libs/GLM"
)

message(STATUS "Configuration complete. Ready to build.")

