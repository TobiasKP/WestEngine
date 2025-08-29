message(STATUS "Building Utils Module")

set(LIB_DEST "${CMAKE_SOURCE_DIR}/Libs/")

project(WestUtils VERSION 0.1.0)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

file(GLOB_RECURSE HEADER_FILES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
  "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
) 

add_library(WestUtils STATIC 
  ${SOURCE_FILES}
  ${HEADER_FILES}
)

target_include_directories(WestUtils
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
    "$<INSTALL_INTERFACE:include>"
)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${LIB_DEST}"
    COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:WestUtils>" "${LIB_DEST}"
)


