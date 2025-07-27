@echo off

echo Starting CMake in build directory ...
:: Navigate to the directory of the batch file

cd /d "%~dp0"

if not exist build (
  mkdir build
)

cd build

echo Cleaning build directory ...
rmdir /s /q .
if errorlevel 1 (
  echo Failed to delete files in the build directory.
  exit /b 1
)

echo Running build
cmake .. -DCMAKE_BUILD_TYPE=Release

if errorlevel 1 (
  echo CMake configuration failed 
  exit /b 1
)

echo Finishing config starting make
cmake --build . --config Release
if errorlevel 1 (
  echo Make failed
  exit /b 1
)

echo Build completed successfully
