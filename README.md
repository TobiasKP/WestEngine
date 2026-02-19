# WestEngine

A C++23 game engine built from scratch, designed for tile-based, turn-style games. It features an Entity-Component-System architecture, OpenGL 3D rendering, Lua scripting, a custom UI framework, and multi-threaded task execution.

This is a personal project developed to explore game engine architecture and low-level systems programming.

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
| Cross-Platform Build | Implemented | CMake + vcpkg, presets for Windows, macOS, Linux |
| Audio | Planned | Not yet implemented |
| Physics/Collision | Not applicable | Engine targets discrete tile-based movement |

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

For detailed architectural decisions and rationale, see [ARCHITECTURE.md](ARCHITECTURE.md).

## Requirements

**Software:**

- C++23 compatible compiler (GCC 13+, Clang 16+, MSVC 19.35+)
- CMake 3.10+
- OpenGL compatible graphics driver ([Getting Started](https://www.khronos.org/opengl/wiki/Getting_Started))

**Platforms:** Windows, macOS, Linux

## Building

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
cmake --preset macos-debug      # or macos-release

# Build
cmake --build --preset <your-preset>
```

Module-specific debug builds are also available (e.g. `linux-debug-core`, `windows-debug-ui`). See `CMakePresets.json` for all options.

The executable `WestCore` will be located in `build/<preset>/WestEngine/WestCore/`.

**Note (Linux):** Write access is required in the application directory for log file generation.

## Dependencies

Managed via [vcpkg](https://learn.microsoft.com/en-us/vcpkg):

| Library | Purpose |
|---------|---------|
| GLEW | OpenGL extension loading |
| GLFW3 | Window and input management |
| Lua | Embedded scripting |
| GLM | Mathematics (vectors, matrices, transforms) |
| STB | Image loading |

## Project Structure

```
WestEngine/
├── WestEngine/          # Core engine (ECS, managers, rendering, scripting)
│   ├── Core/            # Implementation files
│   ├── CoreHeaders/     # Header files
│   ├── Constants/       # Engine configuration
│   └── Shader/          # GLSL shaders (vertex, fragment, world)
├── WestInterface/       # UI framework (elements, observers, rendering)
│   ├── WestInterface/   # Source and headers
│   ├── Shader/          # UI-specific shaders
│   └── Resources/       # Fonts and icons
├── WestUtils/           # Shared utilities (threading, memory, logging)
│   └── Include/         # Public headers
├── WestGame/            # Game content (not compiled)
│   ├── Scripts/         # Lua game scripts
│   ├── Assets/          # Models and textures
│   └── Config/          # Game settings and keybindings
├── CMakeLists.txt       # Root build configuration
└── CMakePresets.json    # Platform-specific build presets
```

## License

This project is licensed under the MIT License. See [LICENSE](LICENSE) for details.
