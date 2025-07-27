#include <GL/glew.h>
#include <cstdint>

namespace CoreConstants {
extern const char *TITLE;

extern const char *INFO_FILE_NAME;
extern const char *ERROR_FILE_NAME;
extern const char *INPUT_CONFIG_FILE_NAME;
extern const char *AVAILABLE_INPUTS_FILE_NAME;
extern const char *DEBUG_V_SHADER;
extern const char *DEBUG_F_SHADER;
extern const char *LUA_INIT_FILE;

extern const std::uint8_t MAX_Q_SIZE;
extern const std::uint16_t MAX_ENTITY_SIZE;
extern const std::uint8_t CHUNK_SIZE;
extern const std::uint16_t TEXT_SHADERGROUP;
extern const std::uint16_t DEBUG_SHADERGROUP;

extern const char *ENGINE_MANAGER;
extern const char *WINDOW_MANAGER;
extern const char *RENDER_MANAGER;
extern const char *INPUT_MANAGER;
extern const char *SCENE_MANAGER;
extern const char *SHADER_MANAGER;
extern const char *ENTITY_SYSTEM_MANAGER;
extern const char *INTERFACE_MANAGER;

} // namespace CoreConstants
