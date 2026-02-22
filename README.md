# WestEngine

A early verion of a C++23 game engine built from scratch, designed for tile-based, turn-style games. It features an Entity-Component-System architecture, OpenGL 3D rendering, Lua scripting, a custom UI framework, and multi-threaded task execution.

This is a personal project developed to explore system architecture, system design, parallel computing and low-level systems programming. 

The project is activly developed further privately. Issues active by the time of creation were transfered into @TODO statements in the code to keep the display repository clean. 

## Requirements

**Software:**

- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 19.35+)
- CMake 3.15+
- OpenGL compatible graphics driver ([Getting Started](https://www.khronos.org/opengl/wiki/Getting_Started))

**Platforms:** 

Linux: Arch v6.15.2, tested both release and debug build sucessfully, with cmake v4.1.1, gcc v15.1.1, nvidia driver v575.57.08, lua v5.4.8. 

Windows: Tested with the latest installation of Visual Studio Community 2026 v18.3.1

## Building

**Note:** Simply cloning the repo into VS Community will be enough to run the project, just be aware that MSVC v143 is needed. 

Clone with submodules (vcpkg is included as a submodule for dependency management):

```bash
git clone --recurse-submodules https://github.com/TobiasKP/WestEngine.git
```

If already cloned without submodules:

```bash
git submodule update --init --recursive
```

Configure and build using CMake presets:

```bash
# Configure (pick your platform)
cmake --preset linux-debug      # or linux-release
cmake --preset windows-debug    # or windows-release

# Build
cmake --build --preset <your-preset>
```

Module-specific debug builds are also available (e.g. `linux-debug-core`, `windows-debug-ui`). See `CMakePresets.json` for all options.

**Note:** You may find MacOs build presets in the CMakePresets. MacOS is currently NOT supported since MacOs stopped OpenGL support after v4.1, future support is planned but not yet implemented or tested, so runconfigs will not work.

The executable `WestCore` will be located in the projects root folder following the path: `build/<preset>/WestEngine/`. 

**Note:** Write access is required in the application directory for log file generation, not having read/write rights may lead to undefined behaviour. 

## Dependencies

Managed via [vcpkg](https://learn.microsoft.com/en-us/vcpkg):

| Library | Purpose |
|---------|---------|
| GLEW | OpenGL extension loading |
| GLFW3 | Window and input management |
| Lua | Embedded scripting |
| GLM | Mathematics (vectors, matrices, transforms) |
| STB | Image loading |

## Features

| System | Status | Description |
|--------|--------|-------------|
| ECS (Entity-Component-System) | Implemented | Bitmask-based component storage, entity pooling, system dispatch |
| OpenGL Rendering | Implemented | Shader management, instanced UI rendering, debug visualization |
| Lua Scripting | Implemented | Bidirectional C++/Lua bridge for game logic and entity definitions |
| UI Framework | Implemented | Buttons, labels, dropdowns, observer-based events, settings interface |
| Tile-Based World | Implemented | Grid coordinate system, Manhattan distance pathfinding, GPU-driven tile rendering |
| Input System | Implemented | Configurable keybindings, keyboard/mouse callbacks, observer pattern |
| Threading | Implemented | Thread pool, fire-and-forget tasks, hardware-aware thread count |
| Memory Pooling | Implemented | Custom pool allocator for entities with free-list management |
| Logging | Implemented | Multi-stream logger (info, error, cycle) with thread-safe writes |
| Cross-Platform Build | Implemented | CMake + vcpkg, presets for Windows and Linux |


## Architecture

The engine is organized into four modules with a strict dependency hierarchy:

```
WestEngine (Core)  ──►  WestUtils (Shared Infrastructure)
       │                         ▲
WestInterface (UI) ──────────────┘

WestGame (Content) ── copied into build output, not compiled
```

- **WestEngine**: ECS, managers, systems, rendering, scripting
- **WestInterface**: UI elements, event/value observers, UI rendering pipeline
- **WestUtils**: Thread pool, pool allocator, logging, configuration
- **WestGame**: Lua scripts, assets, configuration (game content)

For some architectural decisions, rationale, known limitations and a more technical discussion, see [ARCHITECTURE.md](ARCHITECTURE.md).

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
