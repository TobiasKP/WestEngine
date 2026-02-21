# Architecture

This document describes the architectural decisions behind WestEngine and the reasoning that shaped them.

## Overview

WestEngine is a C++23 game engine built around a modular manager architecture with an Entity-Component-System (ECS) core. It targets tile-based, turn-style games and provides rendering (OpenGL), scripting (Lua), a custom UI framework, and multi-threaded task execution.

```
                         ┌──────────────────┐
                         │   WestEngine.cpp  │
                         │   (Entry Point)   │
                         └────────┬─────────┘
                                  │
                         ┌────────▼─────────┐
                         │  EngineManager    │
                         │  (Orchestrator)   │
                         └────────┬─────────┘
                                  │
          ┌───────────────────────┼───────────────────────┐
          │                       │                       │
  ┌───────▼───────┐     ┌────────▼────────┐    ┌─────────▼────────┐
  │  WestEngine   │     │  WestInterface  │    │    WestUtils      │
  │  (Core)       │     │  (UI)           │    │    (Shared Libs)  │
  │               │     │                 │    │                   │
  │ - ECS         │     │ - Elements      │    │ - ThreadPool      │
  │ - Managers    │     │ - Observer      │    │ - PoolAllocator   │
  │ - Systems     │     │ - Rendering     │    │ - Logging         │
  │ - Scripting   │     │ - Facade        │    │ - Config          │
  │ - Rendering   │     │                 │    │                   │
  └───────────────┘     └─────────────────┘    └───────────────────┘
          │                                              │
          └──────────────────────┬───────────────────────┘
                                 │
                        ┌────────▼─────────┐
                        │    WestGame      │
                        │  (Game Content)  │
                        │                  │
                        │ - Lua Scripts    │
                        │ - Assets         │
                        │ - Config         │
                        └──────────────────┘
```

## Module Boundaries

The codebase is split into four modules. The separation follows a dependency rule: modules only depend downward, never upward or sideways without going through a defined interface.

**WestUtils** is the foundation layer. It provides infrastructure that any module may need: thread pool, pool allocator, logging, configuration, and shared constants. It has zero dependencies on the other modules.

**WestEngine (Core)** contains the engine itself: the ECS, all managers, game systems, rendering pipeline, input handling, and the Lua scripting bridge. It depends on WestUtils and WestInterface through a facade.

**WestInterface** is the UI framework. It manages its own rendering pipeline (separate shaders, separate VAO/VBO), UI elements (buttons, labels, dropdowns), and an observer-based event system. It is accessed through a facade so the core engine does not depend on UI internals.

**WestGame** is the game content layer. It contains Lua scripts, assets, and configuration files. It is not yet compiled and is copied into the build output at post-build time. This keeps game-specific content fully outside the engine source.

The reason for this split rather than a monolithic structure: each module can be built and reasoned about independently. The UI framework, for example, could be replaced without touching the ECS or rendering core. Also recompiling all modules all over or a monolith is just annoying.

## Why ECS Over Inheritance Hierarchies

A traditional deep inheritance tree (e.g. `GameObject → Character → Player`) creates rigid coupling. Adding a new behavior means modifying the hierarchy or using multiple inheritance, both of which scale poorly. The component approach used here keeps entities as lightweight containers. An `Entity` holds an ID, a component bitmask (`uint16_t`), and a map of components. Behavior is defined by which components are attached, not by class lineage. Systems iterate over entities that match their required component mask.

This makes it straightforward to define new entity types entirely in Lua without touching C++ code. A "player" and a "decoration" are the same `Entity` class — they differ only in their component set.

The bitmask approach was chosen over a full archetype-based ECS (like EnTT) to keep the implementation transparent and debuggable at the cost of a 16-component limit for the current state of the engine and is planned to be removed in the near future (look at known limitations).

## Why Lua for Scripting

The engine uses Lua as its scripting layer, accessed through `LuaFacade`. The choice came down to three factors:

1. **Embeddability**: Lua is designed to be embedded in C/C++ applications. The entire runtime is a single library with a clean C API. There is no heavy runtime to initialize or manage.
2. **Game industry precedent**: Lua is widely used in game engines (World of Warcraft, Roblox, CryEngine). Using it signals familiarity with industry tooling.
3. **Lightweight footprint**: For a solo-developed engine, a scripting layer needs to be simple enough to integrate and maintain alone. Python or C# would require significantly more bridging infrastructure.

Game entities are defined in YAML files, loaded by Lua scripts, and instantiated as C++ `Entity` objects. Game logic (state changes, click handlers, movement triggers) lives in Lua. Core systems (rendering, physics, input) stay in C++.

## Why OpenGL Over Vulkan

OpenGL was chosen as the graphics API for practical reasons:

- **Scope management**: Vulkan requires significantly more boilerplate (pipeline state objects, command buffers, synchronization primitives). For a solo project focused on demonstrating architecture rather than cutting-edge graphics, that complexity does not pay off.
- **Cross-platform simplicity**: OpenGL with GLFW provides a working rendering context on Windows, macOS, and Linux with minimal platform-specific code.

The rendering pipeline uses separate shader programs for world geometry, game entities, UI elements, and debug visualization. Shader management is centralized in `ShaderManager`, which handles compilation, linking, and uniform management.

## Manager Lifecycle

All engine subsystems implement the `IManager` interface and follow a four-phase lifecycle:

```
startup()  →  init()  →  update() (per frame)  →  shutdown()
```

- **startup()**: Load configuration, allocate resources, set up prerequisites. No cross-manager dependencies at this stage.
- **init()**: Initialize state that depends on other managers being started. This is where cross-system wiring happens.
- **update()**: Called every frame. Each manager performs its per-frame work.
- **shutdown()**: Release resources in reverse order.

The `EngineManager` holds all seven managers in a fixed-order array and iterates them sequentially:

```
InputManager → WindowManager → ShaderManager → SystemManager
    → InterfaceManager → RenderManager → SceneManager
```

This order is intentional: input must be polled before systems process it, shaders must be compiled before rendering uses them, and scene changes happen last to avoid mid-frame inconsistency. This approach comes with the downside of a highly knowledgeable god class of the EngineManager and some coupling that should not be there, e.g. the WindowManager dependency in other places. This is a known issue. So far it is not being adressed since the gain at current stage would be minimal and simplicity given a scaling project makes reading easier. If need arises (or time for a refactoring is found) the plan is to implement an event driven approach for all managers.

## Known Limitations

- **No physics engine**: The engine targets turn-based tile games where collision detection and rigid body dynamics are unnecessary. Movement is discrete (tile-to-tile), not continuous. But a new module is in planning.
- **No audio system**: Not yet implemented. Would integrate via a lightweight library (e.g. miniaudio or OpenAL). It is planned with ffmpeg for video streaming as a future epic task.
- **No skeletal animation**: Current entities use static meshes. Animation support would require a bone/keyframe system. Given the state of the engine it is not necessary yet.
- **16-component limit**: The bitmask approach caps component types at 16. Sufficient for current scope, but would need migration to a bitset or archetype system for larger projects. It is one of the most urgent fixes needed for the project.
- **Single-scene model**: One active scene at a time. Scene transitions require full unload/reload.
- **No asset pipeline**: Models are loaded from OBJ files at startup. A production engine would need streaming, caching, and format optimization.
- **Result types over integer error codes**: `startup()` as well as all other functions throughout the project return `int32_t` for success/failure. My idea was avoiding try catch or error overhead. Avoiding this overhead was a delibirate choice for a frame-loop engine.

## What I would change if i had to start new

- **Smart pointers over raw `new`/`delete`**: The codebase uses manual memory management in several places. `std::unique_ptr` for manager ownership and component storage would make lifetime guarantees explicit. This limitation arose from my background as a main Java developer - when startin with the project i had to learn c++, the codebase predates my familiarity with modern c++. Once i learned about the modern version of pointers i decided it was too late to change the whole project and stuck with raw pointer use. If a refactoring of the whole code is feasable in the future due to more complex growing memory issues choosing shared or unique pointers is obvious.
- **Type-safe component access**: The current C-style casts for component retrieval (`(Position*)getComponent(POSITION)`) work but bypass the type system. A template-based accessor (`getComponent<Position>()`) would be safer, by the time of creation i was not yet comfortable enough with c++ templates to dare the jump, it is part of the roadmap though, since the bitmap has to be replaced anyway.
- **Consistent header extensions**: The mix of `.h` and `.hpp` is unintentional. A single convention would be cleaner. .hpp is prefered.
- **Manager Queue**: Causes a lot of coupling and is a very rigid approach, no parallel execution of systems that could benefit from it. For a low scale project it is enough but does not scale well with more Managers being added (Audio,Physic,Animation to name a few).
- **Self written UI**: For learning purposes i decided to write my own UI lib instead of using something like imGUI which would have been an easy obvious choice here. Doint it once myself was an interesting experience. Planning out the systems design was nice but it comes with a lot of work. Looking back it was probably not the smartest idea and if i had to start new i would always use imGUI and recommend using it to anyone planning on using this base product. I have learned a lot but you should not reinvent the wheel.- **Adding not needed Features**: For example some of the already existing thread safety is not needed and currently only adds boilerplate code. I implemented things before the need was there. Looking to far into a not existing future comes with costs today. Always solve a problem once you have it and don't solve problems you don't have yet. If starting new i would wait until the engine runs into performance issues, find the bottleneck and solve it with concurrency at that point in time and not try to optimize 20% before its needed. 

