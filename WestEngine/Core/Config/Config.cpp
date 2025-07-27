#include "Config.h"

#include <../../../Libs/GLM/ext/matrix_clip_space.hpp>
#include <../../../Libs/GLM/glm.hpp>
#include <GL/glew.h>
/*
namespace Global {
GLint WIDTH = 1920;
GLint HEIGHT = 1080;
bool PAUSE = false;
ThreadPool *THREADPOOL = new ThreadPool(4);
float SPEED = 0.05f;
float EPSILON = 1e-6f;
std::atomic<std::uint32_t> INTERNAL_ENTITY_ID = 900000;
std::atomic<std::uint32_t> INTERNAL_ID = 800000;
std::atomic<std::int32_t> CAMERA_X = 0, CAMERA_Y = 0;
std::atomic<glm::vec3> PLAYER_DESTINATION = glm::vec3(0.0f);
namespace UserInterface {
glm::mat4 ORTHO_MATRIX =
    glm::ortho(0.0f, (float)Global::WIDTH, 0.0f, (float)Global::HEIGHT);
GLuint SHADER_PROGRAM = -1;
GLuint ORTHO_UNIFORM = -1;
GLuint TEXTURE_SAMPLER = -1;
} // namespace UserInterface
}; // namespace Global
//
//*/
//

namespace Config {
Internals EngineInternals;
General GeneralConfig;
UserInterface Interface;
} // namespace Config
