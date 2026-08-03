cmake_minimum_required(VERSION 3.15)
cmake_policy(SET CMP0091 NEW)
project(WestEngine VERSION 0.3.3)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "C++ Standard = ${CMAKE_CXX_STANDARD}")

# Ensure all MSVC targets use the same CRT (dynamic by default). This avoids
# linker errors caused by mixing /MT and /MD runtimes across objects.
if(MSVC)
  # Use MD / MDd (MultiThreaded DLL) for release/debug respectively.
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL" CACHE STRING "MSVC runtime library" FORCE)
endif()

option(BUILD_CORE "Build Core Module" ON)
option(BUILD_UTILS "Build Utility Modul" OFF)
option(BUILD_UI "Build User Interface Module" OFF)
option(BUILD_DATA "Build data pipeline" OFF)
option(BUILD_TESTS "Build Unit Tests" OFF)
option(BUILD_RENDERER "Build Renderer" OFF)

if(BUILD_UTILS) 
  add_subdirectory(WestUtils/)
endif()

if(BUILD_UI)
  add_subdirectory(WestInterface/)
endif()

if(BUILD_DATA)
  add_subdirectory(WestData/)
endif()

if(BUILD_RENDERER) 
  add_subdirectory(WestRenderer/)
endif()

if(BUILD_CORE)
  add_subdirectory(WestEngine/)
endif()

if(BUILD_TESTS)
  enable_testing()
  add_subdirectory(Tests/)
endif()

