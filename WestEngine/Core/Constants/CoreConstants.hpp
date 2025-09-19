#pragma once

#include <GL/glew.h>
#include <cstdint>
#include <string>

//TODO make everything here
namespace CoreConstants {
static constexpr std::string TITLE = "Test Title";
static constexpr std::string UNDEFINED_STRING = "";

static constexpr char INPUT_CONFIG_FILE_NAME[] = "/config/Game.ini";
static constexpr char AVAILABLE_INPUTS_FILE_NAME[] = "/engine/AvailableInputCommands.cfg";
static constexpr char DEBUG_V_SHADER[] = "/shader/Debug/DebugVShader.vs";
static constexpr char DEBUG_F_SHADER[] = "/shader/Debug/DebugFShader.fs";
static constexpr std::string LUA_INIT_FILE = "/lua/Main.lua";

static constexpr std::uint8_t MAX_Q_SIZE = 7;
static constexpr std::uint16_t MAX_ENTITY_SIZE = 512;
static constexpr std::uint8_t CHUNK_SIZE = 64;
static constexpr std::uint16_t DEBUG_SHADERGROUP = 999;

static constexpr std::string ENGINE_MANAGER = "ENGINGE_MANAGER";
static constexpr std::string WINDOW_MANAGER = "WINDOW_MANAGER";
static constexpr std::string RENDER_MANAGER = "RENDER_MANAGER";
static constexpr std::string INPUT_MANAGER = "INPUT_MANAGER";
static constexpr std::string SCENE_MANAGER = "SCENE_MANAGER";
static constexpr std::string SHADER_MANAGER = "SHADER_MANAGER";
static constexpr std::string ENTITY_SYSTEM_MANAGER = "SYSTEM_MANAGER";
static constexpr std::string INTERFACE_MANAGER = "UI_MANAGER";

} // namespace CoreConstants
