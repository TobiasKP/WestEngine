#pragma once

#include <GL/glew.h>
#include <cstdint>
#include <string>

namespace CoreConstants {
inline constexpr std::string TITLE = "Test Title";
inline constexpr std::string UNDEFINED_STRING = "";

inline constexpr char INPUT_CONFIG_FILE_NAME[] = "/config/Game.ini";
inline constexpr char AVAILABLE_INPUTS_FILE_NAME[] =
    "/engine/AvailableInputCommands.cfg";
inline constexpr char DEBUG_V_SHADER[] = "/shader/Debug/DebugVShader.vs";
inline constexpr char DEBUG_F_SHADER[] = "/shader/Debug/DebugFShader.fs";
inline constexpr std::string LUA_INIT_FILE = "/lua/Main.lua";

inline constexpr std::uint8_t MAX_Q_SIZE = 7;
inline constexpr std::uint16_t MAX_ENTITY_SIZE = 512;
inline constexpr std::uint8_t CHUNK_SIZE = 64;
inline constexpr std::uint16_t DEBUG_SHADERGROUP = 999;

inline constexpr std::string ENGINE_MANAGER = "ENGINGE_MANAGER";
inline constexpr std::string WINDOW_MANAGER = "WINDOW_MANAGER";
inline constexpr std::string RENDER_MANAGER = "RENDER_MANAGER";
inline constexpr std::string INPUT_MANAGER = "INPUT_MANAGER";
inline constexpr std::string SCENE_MANAGER = "SCENE_MANAGER";
inline constexpr std::string SHADER_MANAGER = "SHADER_MANAGER";
inline constexpr std::string ENTITY_SYSTEM_MANAGER = "SYSTEM_MANAGER";
inline constexpr std::string INTERFACE_MANAGER = "UI_MANAGER";

} // namespace CoreConstants
