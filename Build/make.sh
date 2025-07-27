#!/bin/bash

echo "=== CORE BUILD ==="
echo "Linux Distribution detected - continuing build process..."
echo "This build requires previous modules in order to work properly..."
echo "Starting CMake in build directory..."

# Navigate to script's directory
cd "$(dirname "$0")" || exit 1

# Ensure Release and Debug directories exist
mkdir -p Release Debug

# Parse flags
DEBUG_FLAG=0
RELEASE_FLAG=0
FULL_BUILD=0

for arg in "$@"; do
  if [[ "$arg" == "--debug" ]]; then
      echo "Building Debug Version"
      DEBUG_FLAG=1
  fi

  if [[ "$arg" == "--release" ]]; then
      echo "Building Release Version"
      RELEASE_FLAG=1
  fi

  if [[ "$arg" == "--buildAll" ]]; then
      echo "Building all Dependencies"
      FULL_BUILD=1
  fi
done

# Optional: Build UserInterface module
if [[ "$FULL_BUILD" == 1 ]]; then
    echo "Building Interface..."
    if [[ -f "../UserInterface/make.sh" ]]; then
        (cd ../UserInterface && bash make.sh)
    else
        echo "../UserInterface/make.sh not found."
    fi
fi

# === RELEASE BUILD ===
if [[ "$RELEASE_FLAG" == 1 ]]; then
    echo "Cleaning Release build directory..."
    rm -rf Release/*

    echo "Configuring Release build..."
    (cd Release && cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..)
    if [[ $? -ne 0 ]]; then
        echo "CMake configuration for Release failed."
        exit 1
    fi

    echo "Building Release..."
    (cd Release && make)
    if [[ $? -ne 0 ]]; then
        echo "Make for Release failed."
        exit 1
    fi

    echo "Cleaning up Release build files..."
    rm -rf Release/CMakeFiles Release/CMakeCache.txt Release/Makefile Release/cmake_install.cmake Release/compile_commands.json

    echo "Editing Lua file: lua/Utils.lua"
    sed -i 's/_DEBUG = true/_DEBUG = false/' Release/lua/Utils.lua

    echo "Release build completed successfully."
fi

# === DEBUG BUILD ===
if [[ "$DEBUG_FLAG" == 1 ]]; then
    echo "Cleaning Debug build directory..."
    rm -rf Debug/*

    echo "Configuring Debug build..."
    (cd Debug && cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug .. --trace-expand)
    if [[ $? -ne 0 ]]; then
        echo "CMake configuration for Debug failed."
        exit 1
    fi

    echo "Building Debug..."
    (cd Debug && make VERBOSE=1)
    if [[ $? -ne 0 ]]; then
        echo "Make for Debug failed."
        exit 1
    fi

    echo "Editing Lua file: Debug/lua/Utils.lua"
    sed -i 's/_DEBUG = false/_DEBUG = true/' Debug/lua/Utils.lua

    echo "Debug build completed successfully."
fi

