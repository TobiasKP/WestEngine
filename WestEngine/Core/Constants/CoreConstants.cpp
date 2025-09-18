#include "CoreConstants.h"

#include <glm/gtc/matrix_transform.hpp>

namespace CoreConstants {
const std::string TITLE = "Test Title";
const std::string UNDEFINED_STRING = "";

const std::string INPUT_CONFIG_FILE_NAME = "/config/Game.ini";
const std::string AVAILABLE_INPUTS_FILE_NAME = "/engine/AvailableInputCommands.cfg";
const std::string LUA_INIT_FILE = "/lua/Main.lua";
const std::string DEBUG_V_SHADER = "/shader/Debug/DebugVShader.vs";
const std::string DEBUG_F_SHADER = "/shader/Debug/DebugFShader.fs";

const std::uint8_t MAX_Q_SIZE = 7;
const std::uint16_t MAX_ENTITY_SIZE = 512;
const std::uint16_t DEBUG_SHADERGROUP = 999;
const std::uint16_t INTERFACE_SHADERGROUP = 998;

const std::string ENGINE_MANAGER = "ENGINE_MANAGER";
const std::string WINDOW_MANAGER = "WINDOW_MANAGER";
const std::string RENDER_MANAGER = "RENDER_MANAGER";
const std::string INPUT_MANAGER = "INPUT_MANAGER";
const std::string SCENE_MANAGER = "SCENE_MANAGER";
const std::string SHADER_MANAGER = "SHADER_MANAGER";
const std::string ENTITY_SYSTEM_MANAGER = "SYSTEM_MANAGER";
const std::string INTERFACE_MANAGER = "INTERFACE_MANAGER";

} // namespace CoreConstants
