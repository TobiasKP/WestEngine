# WestEngine Project Documentation

## Overview

WestEngine is a cross-platform 3D game engine written in modern C++23, designed with a modular architecture and manager-based design patterns. The engine provides a complete framework for game development, including rendering, input handling, scene management, entity-component systems, and user interface capabilities.

## Project Purpose

WestEngine aims to deliver a high-performance, extensible game engine that supports:
- 3D graphics rendering with OpenGL
- Entity-Component-System (ECS) architecture
- Scene and asset management
- User interface system with custom rendering
- Cross-platform compatibility (Linux, Windows, macOS)
- Lua scripting integration
- Multi-threaded operations with custom threading utilities

## Core Technologies

### Programming Language & Standards
- **C++23** - Modern C++ with latest language features
- **CMake 3.10+** - Cross-platform build system with preset configurations
- **vcpkg** - Dependency management for third-party libraries

### Graphics & Rendering
- **OpenGL** - Primary graphics API for 3D rendering
- **GLEW** (OpenGL Extension Wrangler) - OpenGL extension loading
- **GLFW3** - Window management and input handling
- **GLM** (OpenGL Mathematics) - Mathematical operations for graphics
- **STB** - Image loading and texture processing

### Scripting & Configuration
- **Lua** - Embedded scripting engine for game logic
- **Custom Configuration System** - INI-based configuration management

### Utilities & Infrastructure
- **Custom Threading** - Thread pool implementation with memory management
- **Custom Logging** - Thread-safe logging system with file output
- **Custom Memory Management** - Pool allocator for optimized memory usage

## Project Architecture

### Modular Design

WestEngine follows a three-tier modular architecture:

1. **WestEngine (Core Module)**
   - Main engine executable and entry point
   - Manager-based architecture coordinating all engine systems
   - Entity-Component-System implementation
   - Rendering pipeline and shader management
   - Input handling and window management

2. **WestUtils (Utility Library)**
   - Shared utility functions and infrastructure
   - Thread-safe logging system
   - Thread pool and memory management
   - Configuration management
   - Time utilities and performance tracking

3. **WestInterface (UI Library)**
   - Custom user interface rendering system
   - Element factory pattern for UI components
   - Observer pattern for UI interactions
   - Text rendering and font management
   - Custom UI shaders and rendering pipeline

### Build Configuration

The project supports flexible build configurations:
- **Individual Module Building** - Build specific modules for faster iteration
- **Cross-Platform Presets** - Predefined configurations for different platforms
- **Debug/Release Variants** - Optimized builds with different feature sets

### Manager-Based Architecture

The engine core uses a manager pattern with centralized coordination:
- **EngineManager** - Central coordinator with lifecycle management
- **WindowManager** - GLFW window and OpenGL context management
- **RenderManager** - Rendering pipeline coordination
- **SceneManager** - Scene loading and management
- **SystemManager** - ECS system coordination
- **ShaderManager** - Shader compilation and management
- **InterfaceManager** - UI system integration
- **InputManager** - Input event handling

### Communication System

Managers communicate through a custom queue system (`WestQ`) enabling:
- Asynchronous message passing between systems
- Centralized update cycles (STARTUP, INIT, UPDATE, LOAD, PAUSE)
- Decoupled system interactions

## Project Structure

```
WestEngine/
├── WestEngine/          # Core engine module
│   ├── Core/           # Manager implementations and engine logic
│   ├── CoreHeaders/    # Public interfaces and headers
│   ├── Constants/      # Engine constants and configurations
│   ├── Game/           # Game-specific content and assets
│   └── Shader/         # GLSL shader files
├── WestUtils/          # Utility library
│   ├── Include/        # Public utility headers
│   ├── Logging/        # Logger implementation
│   └── ThreadPool/     # Thread management utilities
├── WestInterface/      # UI system library
│   ├── WestInterface/  # UI implementation
│   ├── Shader/         # UI-specific shaders
│   └── Resources/      # UI fonts and resources
└── Build System/       # CMake configuration and presets
```

## Key Features

### Cross-Platform Support
- Linux (Unix Makefiles)
- Windows (Visual Studio 2022)
- macOS (Unix Makefiles with frameworks)

### Asset Pipeline
- Automatic resource copying during build
- Support for Lua scripts, 3D models, textures, and configuration files
- Organized asset structure with game-specific content

### Development Tools
- Configurable build presets for rapid development
- Debug utilities and performance tracking
- Custom slash commands for common operations
- Comprehensive logging with multiple output levels

### Performance Optimizations
- Custom memory pool allocators
- Multi-threaded architecture with thread pools
- Efficient rendering pipeline with OpenGL optimization
- Manager-based update cycles for coordinated system updates

## Getting Started

### Prerequisites
- CMake 3.10 or higher
- C++23 compatible compiler
- vcpkg package manager
- OpenGL 3.3+ compatible graphics card

### Quick Build
```bash
# Configure and build for Linux Debug
cmake --preset linux-debug
cmake --build --preset linux-debug

# Run the engine
./build/linux-debug/WestEngine/WestCore/WestCore
```

For detailed build instructions, module-specific documentation, and development guidelines, refer to the individual module documentation and the CLAUDE.md development guide.