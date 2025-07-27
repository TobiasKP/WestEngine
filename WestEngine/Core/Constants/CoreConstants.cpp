#include "CoreConstants.h"

#include <../../../Libs/GLM/gtc/matrix_transform.hpp>

namespace CoreConstants {
const char *TITLE = "Test Title";

const char *INFO_FILE_NAME = "WestLog.log";
const char *ERROR_FILE_NAME = "WestError.log";


const char *INPUT_CONFIG_FILE_NAME = "/config/Game.ini";
const char *AVAILABLE_INPUTS_FILE_NAME = "/engine/AvailableInputCommands.cfg";
const char *LUA_INIT_FILE = "/lua/Main.lua";
const char *DEBUG_V_SHADER = "/shader/Debug/DebugVShader.vs";
const char *DEBUG_F_SHADER = "/shader/Debug/DebugFShader.fs";

const std::uint8_t MAX_Q_SIZE = 7;
const std::uint16_t MAX_ENTITY_SIZE = 512;
const std::uint8_t CHUNK_SIZE = 64;
const std::uint16_t TEXT_SHADERGROUP = 998;
const std::uint16_t DEBUG_SHADERGROUP = 999;

const char *ENGINE_MANAGER = "ENGINE_MANAGER";
const char *WINDOW_MANAGER = "WINDOW_MANAGER";
const char *RENDER_MANAGER = "RENDER_MANAGER";
const char *INPUT_MANAGER = "INPUT_MANAGER";
const char *SCENE_MANAGER = "SCENE_MANAGER";
const char *SHADER_MANAGER = "SHADER_MANAGER";
const char *ENTITY_SYSTEM_MANAGER = "SYSTEM_MANAGER";
const char *INTERFACE_MANAGER = "INTERFACE_MANAGER";

} // namespace CoreConstants
