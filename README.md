# Installation Process

1: [Requirements](#requirements) <br />
2: [Installation Guide](#installation) <br />
3: [Project Documentation](#documentation) <br />

## Requirements

Requirements for the project to build and run correctly:

OS: MacOS, Windows, Linux <br />
OpenGL suitable graphics driver: [OpenGL introduction](www.khronos.org/opengl/wiki/Getting_Started) <br />
C++ 20 (or later) & compiler (e.g. gcc): [C++ introduction](https://cplusplus.com/doc/tutorial/introduction) <br />
CMake: [Cmake](https://cmake.org) <br />

## Installation

Clone the repository with all submodules. [VCPGK](https://learn.microsoft.com/en-us/vcpkg) is used as a package manager and is required for all further dependencies to be loaded.

*git clone --recurse-submodules `https://github.com/TobiasKP/WestEngine.git`*

if already cloned without recourse option specified:

*git submodule update --init --recursive*

For building the project CMakePresets are given and listed in the CMakePresets.json. Following the two major options for first time building the project.

| Mode                   | Windows                            | Linux                          | MacOS                           |
|:-----------------------|:----------------------------------:|:------------------------------:|--------------------------------:|
| Full Build Cycle       | windows-release                    | linux-release                  | macos-release                   |
| Full Debug Cycle       | windows-debug                      | linux-debug                    | macos-debug                     |
| Module                 | windows-debug-*module name*        | linux-debug-*module name*      | macos-debug-*module name*       |
| Scripts                | TBD                                | TBD                            | TBD                             |

Examples:

setting the full project preset on a linux system: *`cmake --preset linux-release`* <br />
setting the core module preset on a windows system: *`cmake --preset windows-debug-core`* <br />

afterwards the project can be build with the specified presets: <br />
`cmake --build --preset *<your preferred preset>`*

For debug builds the executable *WestCore* will be located in: `${CMAKE_SOURCE_DIR}/build/<used preset>/WestEngine/WestCore/`

For release builds: TBD...

Linux side not: Write access for the folder running the application from is required for log files to be generated. 

## Documentation
