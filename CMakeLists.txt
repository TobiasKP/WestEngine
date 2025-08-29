cmake_minimum_required(VERSION 3.10)
project(WestEngine VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
message(STATUS "C++ Standard = ${CMAKE_CXX_STANDARD}")

option(BUILD_CORE "Build Core Module" ON)
option(BUILD_UTILS "Build Utility Modul" OFF)
option(BUILD_UI "Build User Interface Module" OFF)

if(BUILD_UTILS) 
  add_subdirectory(WestUtils/)
endif()

if(BUILD_UI)
  add_subdirectory(UserInterface/)
endif()

if(BUILD_CORE)
  add_subdirectory(WestEngine/)
endif()

