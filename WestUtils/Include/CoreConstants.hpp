#pragma once

#include <cstdint>
#include <string_view>

namespace CoreConstants
{
inline constexpr std::string_view TITLE            = "Test Title";
inline constexpr std::string_view UNDEFINED_STRING = "";

inline constexpr char INPUT_CONFIG_FILE_NAME[]     = "/config/Game.ini";
inline constexpr char SETTING_FILE_NAME[]          = "/config/Settings.cfg";
inline constexpr char AVAILABLE_INPUTS_FILE_NAME[] = "/engine/AvailableInputCommands.cfg";
inline constexpr char DEBUG_V_SHADER[]             = "/shader/Debug/DebugVShader.vs";
inline constexpr char DEBUG_F_SHADER[]             = "/shader/Debug/DebugFShader.fs";
inline constexpr std::string_view LUA_INIT_FILE    = "/lua/Main.lua";
inline constexpr char LUA_API_FILE[]               = "/lua/WestAPI.lua";

inline constexpr std::uint8_t MAX_Q_SIZE         = 7;
inline constexpr std::uint16_t MAX_ENTITY_SIZE   = 512;
inline constexpr std::uint8_t CHUNK_SIZE         = 64;
inline constexpr std::uint16_t DEBUG_SHADERGROUP = 999;
inline constexpr std::uint16_t WORLD_SHADERGROUP = 1000;

inline constexpr std::string_view ENGINE_MANAGER        = "ENGINGE_MANAGER";
inline constexpr std::string_view WINDOW_MANAGER        = "WINDOW_MANAGER";
inline constexpr std::string_view RENDER_MANAGER        = "RENDER_MANAGER";
inline constexpr std::string_view INPUT_MANAGER         = "INPUT_MANAGER";
inline constexpr std::string_view SCENE_MANAGER         = "SCENE_MANAGER";
inline constexpr std::string_view SHADER_MANAGER        = "SHADER_MANAGER";
inline constexpr std::string_view ENTITY_SYSTEM_MANAGER = "SYSTEM_MANAGER";
inline constexpr std::string_view INTERFACE_MANAGER     = "INTERFA_MANAGER";

}  // namespace CoreConstants
