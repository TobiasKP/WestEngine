#!/bin/bash

echo "=== Starting CMake in build directory ==="
echo "Building Interface library"

# Navigate to the script's directory
cd "$(dirname "$0")" || exit 1

# Create build directory if it doesn't exist
mkdir -p build

cd build || exit 1

echo "Cleaning build directory ..."
rm -rf ./*

if [[ $? -ne 0 ]]; then
  echo "Failed to delete files in the build directory."
  exit 1
fi

echo "Running CMake configuration ..."

# Configure the project for Release mode (static + shared)
cmake .. \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON

if [[ $? -ne 0 ]]; then
  echo "CMake configuration failed"
  exit 1
fi

echo "Starting build with make ..."
cmake --build . --config Release -j$(nproc)

if [[ $? -ne 0 ]]; then
  echo "Build failed"
  exit 1
fi

echo "Build completed successfully!"

