
@echo off
:: Define source and target directories
set "SOURCE_SCRIPTS=%~dp0WestEngine\Core\Game\Scripts"
set "TARGET_SCRIPTS=%~dp0Debug\lua"
set "TARGETR_SCRIPTS=%~dp0Release\lua"

set "SOURCE_ASSETS=%~dp0WestEngine\Core\Game\Assets"
set "TARGET_ASSETS=%~dp0Debug\assets"
set "TARGETR_ASSETS=%~dp0Release\assets"

set "SOURCE_SHADERS=%~dp0WestEngine\Core\Shader"
set "TARGET_SHADERS=%~dp0Debug\shader"
set "TARGETR_SHADERS=%~dp0Release\shader"

:: Create directories and copy files

:: Copy Scripts
if not exist "%TARGET_SCRIPTS%" (
    echo Creating directory: %TARGET_SCRIPTS%
    mkdir "%TARGET_SCRIPTS%"
)
echo Copying script files...
xcopy "%SOURCE_SCRIPTS%" "%TARGET_SCRIPTS%" /E /H /C /I /Y
xcopy "%SOURCE_SCRIPTS%" "%TARGETR_SCRIPTS%" /E /H /C /I /Y

set "LUA_FILE=%~dp0Release\lua\Utils.lua"
if exist "%LUA_FILE%" (
    echo Editing Lua file: %LUA_FILE%
    powershell -Command "(Get-Content -Path '%LUA_FILE%') -replace '_DEBUG = true', '_DEBUG = false' | Set-Content -Path '%LUA_FILE%'"
)


:: Copy Assets
if not exist "%TARGET_ASSETS%" (
    echo Creating directory: %TARGET_ASSETS%
    mkdir "%TARGET_ASSETS%"
)
echo Copying asset files...
xcopy "%SOURCE_ASSETS%" "%TARGET_ASSETS%" /E /H /C /I /Y
xcopy "%SOURCE_ASSETS%" "%TARGETR_ASSETS%" /E /H /C /I /Y

:: Copy Shaders
if not exist "%TARGET_SHADERS%" (
    echo Creating directory: %TARGET_SHADERS%
    mkdir "%TARGET_SHADERS%"
)
echo Copying shader files...
xcopy "%SOURCE_SHADERS%" "%TARGET_SHADERS%" /E /H /C /I /Y
xcopy "%SOURCE_SHADERS%" "%TARGETR_SHADERS%" /E /H /C /I /Y

echo All files have been copied successfully.

