message(STATUS "Building Core Module")

project(WestCore VERSION 0.1.0)

# ─────────────────────────────────────────────────────────────
# Packages
# ─────────────────────────────────────────────────────────────
find_package(GLEW REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(Lua REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(assimp CONFIG REQUIRED)
find_package(Stb REQUIRED)


# ─────────────────────────────────────────────────────────────
# Custom static libraries 
# ─────────────────────────────────────────────────────────────
set(MODULES_DIR "${CMAKE_SOURCE_DIR}/Libs")
set(WESTUTILS_ARCHIVE
  "${MODULES_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}WestUtils${CMAKE_STATIC_LIBRARY_SUFFIX}"
)

add_library(WestUtilities STATIC IMPORTED GLOBAL)
set_target_properties(WestUtilities PROPERTIES
  IMPORTED_LOCATION
    "${WESTUTILS_ARCHIVE}"
  INTERFACE_INCLUDE_DIRECTORIES
    "${CMAKE_SOURCE_DIR}/WestUtils/Include"
)

# ─────────────────────────────────────────────────────────────
# Includes (shared across platforms)
# ─────────────────────────────────────────────────────────────
message(STATUS "Including headers ...")
include_directories(
    ${LUA_INCLUDE_DIR}
    ${Stb_INCLUDE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/CoreHeaders   
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Libs/WEST_INTERFACE
)

# ─────────────────────────────────────────────────────────────
# Platform-specific configuration
# ─────────────────────────────────────────────────────────────
if (WIN32)
    set(PLATFORM "windows")   
    #set(WEST_INTERFACE_LIB WestInterface)
    set(OPENGL_LIB opengl32)
    add_definitions(-DGLEW_STATIC -DLUA_STATIC) 
elseif(APPLE)
    set(PLATFORM "mac")   
    #set(WEST_INTERFACE_LIB WestInterface)
    set(OPENGL_LIB "-framework OpenGL -framework Cocoa -framework IOKit")
elseif(UNIX)
    set(PLATFORM "linux")   
    #set(WEST_INTERFACE_LIB WestInterface)
    set(OPENGL_LIB GL)
endif()

# ─────────────────────────────────────────────────────────────
# Link Directories and Targets
# ─────────────────────────────────────────────────────────────
message(STATUS "Linking libraries for platform: ${PLATFORM}")
link_directories(   
    #${CMAKE_SOURCE_DIR}/Core/Libs/WEST_INTERFACE/${PLATFORM}/lib
)

file(GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Core/*.cpp)

add_executable(${PROJECT_NAME} ${SOURCE_FILES})

target_link_libraries(${PROJECT_NAME} 
    PRIVATE
    WestUtilities 
    GLEW::GLEW
    glfw
    ${LUA_LIBRARIES}
    glm::glm
    assimp::assimp 
    ${OPENGL_LIB}
    #${WEST_INTERFACE_LIB} 
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
    $<$<CONFIG:DEBUG>:DEBUG>
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

file(GLOB CONFIG_FILES "${CMAKE_CURRENT_SOURCE_DIR}/Core/Game/Config/*")

# General file copy post-build
foreach(FOLDER lua assets shader engine config libs libs/STB libs/GLM)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${RESOURCE_BASE}/${FOLDER}"
        COMMENT "Creating ${FOLDER} directory"
    )
endforeach()

# ─────────────────────────────────────────────────────────────
# Post-build file copying of custom modules
# ─────────────────────────────────────────────────────────────
if (WIN32)
    foreach(DLL_NAME 
        #WestInterface.dll
    )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Core/Libs/*/${PLATFORM}/lib/${DLL_NAME}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${DLL_NAME} to build output"
        )
    endforeach()
elseif(UNIX)
    foreach(SO_NAME     
        #WestInterface.so
    )
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${CMAKE_CURRENT_SOURCE_DIR}/Core/Libs/*/${PLATFORM}/lib/${SO_NAME}"
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
            COMMENT "Copying ${SO_NAME} to build output"
        )
    endforeach()
endif()

# ─────────────────────────────────────────────────────────────
# Additional data copying (scripts, assets, configs, shaders)
# ─────────────────────────────────────────────────────────────

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CMAKE_CURRENT_SOURCE_DIR}/Core/Constants/AvailableInputCommands.cfg" "${ENGINE_DIR}"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Core/Game/Scripts" "${RESOURCE_BASE}/lua"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Core/Game/Assets" "${RESOURCE_BASE}/assets"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory "${CMAKE_CURRENT_SOURCE_DIR}/Core/Shader" "${RESOURCE_BASE}/shader"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different ${CONFIG_FILES} "${RESOURCE_BASE}/config"
)

message(STATUS "Configuration complete. Ready to build.")

