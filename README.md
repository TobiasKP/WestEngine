# Installation Process

1: [Section Link](#requirements)
2: [Section Link](#installation)

## Requirements

Requirements for the project to build and run correctly:

OS: MacOS, Windows, Linux
OpenGL suitable graphics driver: [with title](www.khronos.org/opengl/wiki/Getting_Started)
C++ 20 (or later) & compiler (e.g. gcc): [with title](https://cplusplus.com/doc/tutorial/introduction)
CMake: [with title](https://cmake.org)

## Installation

Clone the repository with all submodules. VCPKG is used as a package manager [with title](https://learn.microsoft.com/en-us/vcpkg) and is required for all further dependencies to be loaded.

***git clone --recurse-submodules https://github.com/TobiasKP/WestEngine.git***

if already cloned without recourse option specified:

***git submodule update --init --recursive***

For building the project CMakePresets are given and listed in the CMakePresets.json. Following the two major options for first time building the project.

| Mode                   | Windows                            | Linux                          | MacOS                           |
|:-----------------------|:----------------------------------:|:------------------------------:|--------------------------------:|
| Full Build Cycle       | windows-release                    | linux-release                  | macos-release                   |
| Full Debug Cycle       | **not specified yet**              | **not specified yet**          | **not specified yet**           |
| Module                 | windows-debug-<module name>        | linux-debug-<module name>      | macos-debug-<module name>       |


Examples:

setting the full project preset on a linux system: cmake --preset linux-release
setting the core module preset on a windows system: cmake --preset windows-debug-core

afterwards the project can be build with the specified presets:
cmake --build --preset <your preferred preset>

The executable *WestCore* will be located in: ${CMAKE_SOURCE_DIR}/build/<used preset>/WestEngine/

Write access in the folder runnint the application from is required for log files to be generated. 
