@echo off
echo !CORE BUILD!
echo this build requires previous modules in order to work properly ...
echo Starting CMake in build directory ...

:: Navigate to the directory of the batch file
cd /d "%~dp0"

:: Ensure Build/Release and Build/Debug directories exist
if not exist Release (
    mkdir Release
)
if not exist Debug (
    mkdir Debug
)

:: Check debugging flag
set "DEBUG_FLAG=0"
set "RELEASE_FLAG=0"
set "FULL_BUILD=0"
if "%~1" == "--debug" (
  set "DEBUG_FLAG=1"
) 
if "%~1" == "--release" (
  set "RELEASE_FLAG=1"
)
if "%~2" == "--buildAll" (
  echo Building all Dependencies
  set "FULL_BUILD=1"
)

if "%FULL_BUILD%"=="1" ( 
  cd ../UserInterface
  echo Building Interface ...
  call make.bat
  cd ../Build
)

if "%RELEASE_FLAG%"=="1" (
  :: Build Release
  cd Release

  :: Remove all files and directories without asking for permission
  echo Cleaning Release build directory...
  rmdir /s /q .
  if errorlevel 1 (
      echo Failed to delete files in the Release build directory.
      exit /b 1
  )

  :: Run CMake with specified parameters for Release
  echo Configuring Release build...
  cmake -G  "Unix Makefiles" -DCMAKE_BUILD_TYPE=Release ..
  if errorlevel 1 (
      echo CMake configuration for Release failed.
      exit /b 1
  )

  echo Finished CMake, starting make for Release...

  :: Run make to build the project for Release
  make
  if errorlevel 1 (
      echo Make for Release failed.
      exit /b 1
  )

  echo Cleaning up release path ... 
  for %%d in ("CMakeFiles" "CMakeCache.txt" "Makefile" "cmake_install.cmake" "compile_commands.json") do (
    if exist "%%~d" (
        if exist "%%~d\" (
            echo Deleting directory: %%~d
            rmdir /s /q "%%~d"
        ) else (
            echo Deleting file: %%~d
            del /q "%%~d"
        )
    )
  ) 
 
  cd lua 
  echo Editing Lua file: "Utils.lua" 
  powershell -Command "(Get-Content -Path 'Utils.lua') -replace '_DEBUG = true', '_DEBUG = false' | Set-Content -Path 'Utils.lua'"
  cd ..

  cd ..
  echo Release build completed successfully.
)

if "%DEBUG_FLAG%"=="1" (
  :: Build Debug
  cd Debug

  :: Remove all files and directories without asking for permission
  echo Cleaning Debug build directory...
  rmdir /s /q .
  if errorlevel 1 (
      echo Failed to delete files in the Debug build directory.
      exit /b 1
  )

  :: Run CMake with specified parameters for Debug
  echo Configuring Debug build...
  cmake -G "Unix Makefiles" -DCMAKE_BUILD_TYPE=Debug .. --trace-expand
  if errorlevel 1 (
      echo CMake configuration for Debug failed.
      exit /b 1
  )

  echo Finished CMake, starting make for Debug...

  :: Run make to build the project for Debug
  make VERBOSE=1
  if errorlevel 1 (
      echo Make for Debug failed.
      exit /b 1
  )
)

echo Debug build completed successfully.

