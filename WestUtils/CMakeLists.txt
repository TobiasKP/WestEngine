message(STATUS "Building Utils Module")

set(LIB_DEST "${CMAKE_SOURCE_DIR}/Libs/")

project(WestUtils VERSION 0.1.0)

file(GLOB_RECURSE SOURCE_FILES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

add_library(WestUtils SHARED ${SOURCE_FILES}) 

target_include_directories(WestUtils
  PUBLIC
    "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Include>"
    "$<INSTALL_INTERFACE:Include>"
)

if(NOT WIN32)
  target_compile_options(WestUtils PRIVATE -fvisibility=hidden)
endif()

target_compile_definitions(WestUtils PRIVATE WESTUTILS_BUILDING_DLL)
