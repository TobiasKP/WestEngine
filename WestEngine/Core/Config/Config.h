#pragma once

#include <glm/glm.hpp>
#include <GL/glew.h>

#include "../CoreHeaders/Utils/DataUtils/ThreadPool.h"

namespace Config {

struct General {
  GLint WIDTH = 800, HEIGHT = 600;
  float SPEED = 0.05f, EPSILON = 1e-6f, FPS = 30.0f; 
};

struct UserInterface {
  char *BITMAP_LOCATION = nullptr;
  char *FRAG_LOCATION = nullptr;
  char *VERTEX_LOCATION = nullptr;
};

struct Internals {
  ThreadPool *THREADPOOL = new ThreadPool(4);
  std::atomic<bool> PAUSE = false;
  std::atomic<std::uint32_t> INTERNAL_ENTITY_ID = 900000;
};

extern Internals EngineInternals;
extern General GeneralConfig;
extern UserInterface Interface;

} // namespace Config

/*
namespace Global {
// TODO bring to other module
namespace UserInterface {
extern glm::mat4 ORTHO_MATRIX;
extern GLuint SHADER_PROGRAM;
extern GLuint ORTHO_UNIFORM;
extern GLuint TEXTURE_SAMPLER;
} // namespace UserInterface

}; // namespace Global
// */
