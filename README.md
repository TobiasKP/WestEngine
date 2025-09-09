# Installation Process

1: [Requirements](#requirements) <br />
2: [Installation Guide](#installation) <br />
3: [Project Documentation](#documentation) <br />

## Requirements

Requirements for the project to build and run correctly:

**Software Requirements**

These are needed for building the project yourself on your machine and are not necessary for the binaries only/release.

OS: MacOS, Windows, Linux <br />
OpenGL suitable graphics driver: [OpenGL introduction](www.khronos.org/opengl/wiki/Getting_Started) <br />
C++ 20 (or later) & compiler (e.g. gcc): [C++ introduction](https://cplusplus.com/doc/tutorial/introduction) <br />
CMake: [Cmake](https://cmake.org) <br />

These instructions will get you to a working cmake & vcpkg enviroment, please ensure that all necessary dependencies (e.g. ninja or zip) are already installed and working, specially on Linux distributions like Arch a lot of manual configuration is required. Also write/executable rights are needed for the repository folder.

**Hardware Requirements:**

|Hardware       | Minimal        | Recommended |
|:--------------|:--------------:|------------:|
|CPU            |                |             |
|GPU            |                |             |
|Memory         |                |             |
|OS Version     |                |             |
|Free Disk Space|                |             |


## Installation

Clone the repository with all submodules. [VCPGK](https://learn.microsoft.com/en-us/vcpkg) is used as a package manager and is required for all further dependencies to be loaded.

*git clone --recurse-submodules `https://github.com/TobiasKP/WestEngine.git`*

if already cloned without recourse option specified:

*git submodule update --init --recursive*

For building the project CMakePresets are given and listed in the CMakePresets.json. For first time building the project choose either a full Debug Cycle or a full Build Cycle. The debug Cycle will build the whole project including all modules and dependencies with debug flag enabled, therefor assertions and additional logs are enabled. Also verbose building is enabled. The normal build cycle will build all modules and dependencies with debug mode disabled.  

| Mode                   | Windows                            | Linux                          | MacOS                           |
|:-----------------------|:----------------------------------:|:------------------------------:|--------------------------------:|
| Full Build Cycle       | windows-release                    | linux-release                  | macos-release                   |
| Full Debug Cycle       | windows-debug                      | linux-debug                    | macos-debug                     |
| Module                 | windows-debug-*module name*        | linux-debug-*module name*      | macos-debug-*module name*       |
| Scripts                |                                    |                                |                                 |

Examples:

setting the full project preset on a linux system: *`cmake --preset linux-release`* <br />
setting the core module preset on a windows system: *`cmake --preset windows-debug-core`* <br />

afterwards the project can be build with the specified presets: <br />
*`cmake --build --preset <your preferred preset>`*

For debug builds the executable *WestCore* will be located in: `${CMAKE_SOURCE_DIR}/build/<used preset>/WestEngine/WestCore/`

For release builds: TBD...

Linux side not: Write access for the folder running the application from is required for log files to be generated. 

## Documentation

### Core Module

### Interface Module

### Utility Module
